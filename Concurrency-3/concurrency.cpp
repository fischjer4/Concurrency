/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define MAX_PROCESSES 3

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

using std::cout;
using std::endl;
using std::string;


struct IndvThread{
	int threadNum;
	void* action;
};

sem_t empty;
sem_t full_slots;
sem_t mutex;
sem_t printer;
int sval;

//int num_processes = 0;

/*
	* Outputs the thread number of the thread whose working
*/
void working(const int &workerNum){
    int waittime = (rand() % 4) + 1;
    sem_wait(&printer);
    cout << workerNum << " is working" << endl;
    cout << "Wait time: " << waittime << endl;
    sem_post(&printer);
    sleep(waittime);
    sem_post(&full_slots);
}
/*
	* Outputs the thread number of the thread whose waiting
*/
void waiting(const int &workerNum){
    sem_wait(&printer);
	cout << workerNum << " is waiting" << endl;
    sem_post(&printer);
}
/*
	* The threads base function
	* Decides whether the thread can work yet or not
*/
void *begin(void *worker){
    intptr_t threadNum = (intptr_t) worker;
    sem_getvalue(&full_slots, &sval);
    if (sval == MAX_PROCESSES) {
        sem_wait(&mutex);
        waiting(threadNum);
        while (sval > 0) {
            sem_wait(&full_slots);
            sem_getvalue(&full_slots, &sval);
        }
        sem_post(&mutex);
        sem_wait(&full_slots);
        working(threadNum);
    }
    else {
        working(threadNum);
    }
    return NULL;
}
/*
	* Set up the workers
*/
void initWorkers(struct IndvThread workers[], const int &numThreads){
	for(int i = 0; i < numThreads; i++){
		workers[i].threadNum = i;
		workers[i].action = (void *) waiting;
	}
}

void cleanUp(pthread_t threads[], struct IndvThread workers[], const int &numThreads){
	for(int i = 0; i < numThreads; i++){
		pthread_join(threads[i], NULL);
	}
	if(threads != NULL)
		delete [] threads;
	if(workers != NULL)		
		delete [] workers;
}

/*
	* Main function
*/
int main(int argc, char *argv[]){
    
    srand( time(NULL) );
    
    // INITIALIZE SEMAPHORES //
    sem_init(&empty, 0, 1);
    sem_init(&full_slots, 0, 0);
    sem_init(&mutex, 0, 1);
    sem_init(&printer, 0, 1);
    
	if(argc != 2){
        sem_wait(&printer);
		cout << "Wrong Format" << endl; 
		cout << "Correct Format: concurrency <num threads>" << endl;
        sem_post(&printer);
	}
	else{
		srand(time(NULL));
		int numThreads = atoi(argv[1]);
        intptr_t i = 0;
		if(numThreads < 1){
            sem_wait(&printer);
			cout << "Please choose at least one thread" << endl;
            sem_post(&printer);
		}
		else{
			/* Set up structures */
			struct IndvThread* workers = new struct IndvThread[numThreads];
			pthread_t* threads = new pthread_t[numThreads];
			initWorkers(workers, numThreads);
			/* Start threads */ 
			for(i = 0; i < numThreads; i++){
                //cout << "i: " << i << endl;
				if( pthread_create(&threads[i], NULL, begin, (void *) i) < 0 ){
                    sem_wait(&printer);
					cout << "Error: "
						 << workers[i].threadNum
						 << " thread could not be initialised ... exiting now"
						 << endl;
                    sem_post(&printer);
				}
			}
			cleanUp(threads, workers, numThreads);
		}
	}

	return 0;
}
