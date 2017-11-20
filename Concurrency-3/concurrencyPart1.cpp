/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PROCESSES 3

using std::cout;
using std::endl;
using std::string;

struct IndvThread{
	int threadNum;
	string action;
	int workWaitTime;
};

sem_t keyHolder;
pthread_mutex_t printer;
pthread_mutex_t needAllKeys; 
pthread_cond_t signalAllKeys;
static int keysOpen;
static int stop = 0;
struct IndvThread* workers = NULL;

/*
	* Prints the status of all threads
*/
void* printAll(void* num){
	int numWorkers = *(int*)num;
	if(workers != NULL){
		while(true){
		    pthread_mutex_lock(&printer);
				cout << "Keys open: " << keysOpen << endl;
				for(int i = 0; i < numWorkers; i++){
						if(workers[i].action == "working"){
							cout << "Thread " << i << " is working for " << workers[i].workWaitTime << "..." << endl;
							workers[i].workWaitTime--;
						}
						else{
							cout << "Thread " << i << " is " << workers[i].action << "..." << endl;
						}
				}
				cout << endl;
			pthread_mutex_unlock(&printer);
			
			/* Print every 1 second */
			sleep(1);
		}
	}
	return NULL;
}
/*
	* The threads base function
	* Decides whether the thread can work yet or not
*/
void* begin(void *worker){
    struct IndvThread* curWorker = (struct IndvThread*)worker;
	
	while(true){
		/* if thread needs to wait till all keys are returned, then wait */
		if(stop){
			pthread_mutex_lock(&needAllKeys);	
				while(stop){
					pthread_cond_wait(&signalAllKeys, &needAllKeys);
				}
			pthread_mutex_unlock(&needAllKeys);	
		}
		/* grab a key */
		sem_wait(&keyHolder);	
			/* if all keys are taken, then make others wait till all are back */
			sem_getvalue(&keyHolder, &keysOpen);
			if(keysOpen == 0){
				stop = 1;
			}
			curWorker->action = "working";
            curWorker->workWaitTime = rand() % 13 + 1;
			sleep(curWorker->workWaitTime);
			curWorker->action = "waiting";
		sem_post(&keyHolder);
		
		/* if all keys are back, let go of the lock */
		sem_getvalue(&keyHolder, &keysOpen);
		if(keysOpen == MAX_PROCESSES){
			pthread_cond_broadcast(&signalAllKeys);
			stop = 0;
		}
	}
    return NULL;
}
/*
	* Set up the workers
*/
void initWorkers(struct IndvThread workers[], const int &numThreads){
	for(int i = 0; i < numThreads; i++){
		workers[i].threadNum = i;
		workers[i].action = "waiting";
	}
}
/*
	* Sets up the semaphore an mutexes
*/
void initLocks(){
	sem_init(&keyHolder, 0, 3);
    pthread_mutex_init(&printer, NULL);
    pthread_mutex_init(&needAllKeys, NULL);
	pthread_cond_init(&signalAllKeys, NULL);
}
/*
	* Frees all allocated memory
*/
void freeMemory(pthread_t threads[], struct IndvThread workers[]){
	if(threads != NULL)
		delete [] threads;
	if(workers != NULL)		
		delete [] workers;
}
/*
	* Joins all running threads
*/
void joinThreads(pthread_t threads[], const int &numThreads){
	for(int i = 0; i < numThreads; i++){
		pthread_join(threads[i], NULL);
	}
}
void cleanUp(pthread_t threads[], struct IndvThread workers[], const int &numThreads){
	joinThreads(threads, numThreads);
	freeMemory(threads, workers);
}

/*
	* Main function
*/
int main(int argc, char *argv[]){
    
    srand( time(NULL) );

	if(argc != 2){
		cout << "Wrong Format" << endl; 
		cout << "Correct Format: part1 <num threads>" << endl;
	}
	else{
		int numThreads = atoi(argv[1]);
		if(numThreads < 4){
			cout << "Please choose at least four threads" << endl;
		}
		else{
			/* Inititlize Lock Constructs */
    		initLocks();
			/* Set up structures */
			workers = new struct IndvThread[numThreads];
			pthread_t* threads = new pthread_t[numThreads];
			pthread_t printerThread;
			initWorkers(workers, numThreads);
			/* Start threads */ 
			if( pthread_create(&printerThread, NULL, printAll, (void*)&numThreads) < 0){
				cout << "Error initializing printer thread ... exiting now" << endl;
				freeMemory(threads, workers);	
				exit(EXIT_FAILURE);
			} 
			else{
				for(int i = 0; i < numThreads; i++){
					if( pthread_create(&threads[i], NULL, begin, (void*)&workers[i]) < 0 ){
						pthread_mutex_lock(&printer);
							cout << "Error: "
								<< workers[i].threadNum
								<< " thread could not be initialised ... exiting now"
								<< endl;
						pthread_mutex_unlock(&printer);
						freeMemory(threads, workers);	
						exit(EXIT_FAILURE);
					}
				}
			}
			pthread_join(printerThread,NULL);
			cleanUp(threads, workers, numThreads);
		}
	}

	return 0;
}
