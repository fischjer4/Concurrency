/******************************************************
FILENAME: concurrency.c
AUTHORS: Omeed Habibelahian & Jeremy Fischer
COURSE: CS 444 OPERATING SYSTEMS II - FALL 2017
DATE LAST UPDATED: October 9, 2017

SOURCES USED:
* POSIX Threads: http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html
* Structs: https://www.tutorialspoint.com/cprogramming/c_structures.htm
* Producer/Consumer Links: https://youtu.be/NuvAjMk9bZ8, http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
* RDRAND Invocation: https://software.intel.com/en-us/articles/intel-digital-random-number-generator-drng-software-implementation-guide (Section 4.2.2)
* uint32_t Variable Type and <stdint.h>: http://www.nongnu.org/avr-libc/user-manual/group__avr__stdint.html
* Passing Pointer to a Function: https://beginnersbook.com/2014/01/c-passing-pointers-to-functions/
* Check if system supports RDRAND: https://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand/150230
* Linear Feedback Shift Register: https://en.wikipedia.org/wiki/Linear-feedback_shift_register, https://stackoverflow.com/questions/7602919/how-do-i-generate-random-numbers-without-rand-function
* srand: http://www.cplusplus.com/reference/cstdlib/srand/
******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include <cpuid.h>
#include <time.h>
#include "mt19937ar.c"
#define BUFF_SIZE 33

/* BUFFER ITEM STRUCTS */
struct item{
      int num;
      int wait_time;
};

/* FUNCTION DECLARATIONS */
int rdrand32_step(uint32_t *);
void* produce_items();
void* consume_items();
int check_supports_rdrand();
//void seed_gen();

/* GLOBAL VARIABLES */
int prod_cnt;
int cons_cnt;
int prod_loops;
int cons_loops;
struct item buffer[BUFF_SIZE];
int filled = 0;
int used = 0;
int loops;
sem_t empty_slots;
sem_t full_slots;
sem_t mutex;

/* RDRAND */
int rdrand32_step(uint32_t *rand){
      unsigned char ok;
      asm volatile("rdrand %0; setc %1"
                   : "=r"(*rand), "=qm"(ok));

      return (int)ok;
}

/* CHECK IF SYSTEM SUPPORTS RDRAND */
int check_supports_rdrand(){
      const unsigned int flag_RDRAND = (1 << 30);
      unsigned int eax;
      unsigned int ebx;
      unsigned int ecx;
      unsigned int edx;

      __cpuid(1, eax, ebx, ecx, edx);

      return ((ecx & flag_RDRAND) == flag_RDRAND);

}

/* PRODUCE ITEMS */
void* produce_items(void *producer_num){
      int i;
      int prod_num = *((int *)producer_num);
      uint32_t rand_sleep;
      uint32_t rand_num;
      uint32_t rand_wait;
      int supports_rdrand;
      int seed;
     
      supports_rdrand = check_supports_rdrand();
      if(supports_rdrand){
            printf("Your system supports RDRAND.\n");
      }
      else{
            printf("Your system does not support RDRAND. "
                   "Mersenne Twister will be used instead.\n");
      }
      for (i = 0; i < prod_loops; i++){
            if(filled < BUFF_SIZE){
                  sem_wait(&empty_slots); //check if empty slot available
                  sem_wait(&mutex);

                  //begin critical area 
                  if(supports_rdrand){
                        do {
                              rdrand32_step(&rand_sleep);
                              rand_sleep = rand_sleep % 10;
                        } while (rand_sleep < 3 || rand_sleep > 7);
                        
                        rdrand32_step(&rand_num);
                        
                        do {
                              rdrand32_step(&rand_wait);
                              rand_wait = rand_wait % 10;
                        } while (rand_sleep < 2 || rand_sleep > 9);
                  }
                  else{
                        seed = rand() % 100000000;
                        init_genrand(seed);
                        do {
                              rand_sleep = genrand_int32() % 10;
                        } while (rand_sleep < 3 || rand_sleep > 7);
                        
                        seed = rand() % 100000000;
			      init_genrand(seed);
                        rand_num = genrand_int32() % 10;
                        seed = rand() % 100000000;;
			      init_genrand(seed);
                        
                        do {
                              rand_wait = genrand_int32() % 10;
                        } while (rand_wait < 2 || rand_wait > 9);
                  }
                  printf("\nProducer '%d' is now sleeping for %d seconds.\n", prod_num, rand_sleep);
                  sleep(rand_sleep);
                  printf("Producer '%d' is now placing {num, wait_time} = {%d, %d} into buffer.\n", prod_num, rand_num, rand_wait);
                  buffer[filled].num = rand_num;
                  buffer[filled].wait_time = rand_wait;
                  filled = (filled + 1) % BUFF_SIZE;
                  //end critical area

                  sem_post(&mutex);
                  sem_post(&full_slots); //let others know there's a full slot
            }
      }
      pthread_exit(0);
}

/* CONSUME ITEMS */
void* consume_items(void *consumer_num){
      int i;
      int cons_num = *((int *)consumer_num);      
      struct item *consumed_item;
      for (i = 0; i < cons_loops; i++){
            if(used >= 0 && used < BUFF_SIZE){
                  sem_wait(&full_slots); //check if there's a full slot
                  sem_wait(&mutex);
                  //begin critical area
                  consumed_item = &buffer[used];
                  if(consumed_item != NULL){
                        printf("\nConsumer '%d' is now sleeping for: %d seconds.\n", cons_num, consumed_item->wait_time);                        
                        sleep(consumed_item->wait_time);                  
                        printf("Consumer '%d' has consumed number: %d\n", cons_num, consumed_item->num);
                        used = (used + 1) % BUFF_SIZE;
                  }     
                  //end critical area            
                  sem_post(&mutex);
                  sem_post(&empty_slots); //let others know there's an empty slot]
            }
      }
      pthread_exit(0);      
}

/* MAIN */
int main(int argc, char **argv){
      srand (time(NULL));      
      // IMPORTANT VARIABLES //
      pthread_t producer[prod_cnt];
      pthread_t consumer[cons_cnt];

      // VALIDATE NUMBER OF ARGUMENTS //
      if (argc < 5){
            fprintf(stderr, "Invalid number of arguments.\n");
            fprintf(stderr, "In addition to executable, must include (in the following order):\n");
            fprintf(stderr, "- number of producer threads\n");
            fprintf(stderr, "- number of consumer threads\n");
            fprintf(stderr, "- number of loops per producer\n");
            fprintf(stderr, "- number of loops per consumer\n");
            exit(1);
      }

      // ASSIGN ARGUMENTS TO VARIABLES //
      prod_cnt = atoi(argv[1]);
      cons_cnt = atoi(argv[2]);
      prod_loops = atoi(argv[3]);
      cons_loops = atoi(argv[4]);

      // INITIALIZE SEMAPHORES //
      sem_init(&empty_slots, 0, 32);
      sem_init(&full_slots, 0, 0);
      sem_init(&mutex, 0, 1);

      // CREATE THREADS AND PRODUCE/CONFSUME ITEMS //
      int i = 0;
      for (i = 0; i < prod_cnt; i++){
            pthread_create(&producer[i], NULL, produce_items, (void*)&i);
      }
      for (i = 0; i < cons_cnt; i++){
            pthread_create(&consumer[i], NULL, consume_items, (void*)&i);
      }

      // BRING THREADS BACK TOGETHER //
      for (i = 0; i < prod_cnt; i++){
            pthread_join(producer[i], NULL);
      }

      for (i = 0; i < cons_cnt; i++){
            pthread_join(consumer[i], NULL);
      }

      // DESTROY SEMAPHORES //
      sem_destroy(&empty_slots);
      sem_destroy(&full_slots);
      sem_destroy(&mutex);

      return 0;
}

