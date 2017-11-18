/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Sources Used:
 * https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/apis/users_14.htm
 * https://stackoverflow.com/questions/25848615/c-printing-cout-overlaps-in-multithreading
 * https://stackoverflow.com/questions/5724171/passing-an-array-by-reference
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
	string action;
	int workWaitTime;
	// void (*action)(const int&);
};

sem_t keyHolder;
pthread_mutex_t printer;
pthread_mutex_t needAllKeys;
pthread_cond_t signalAllKeys;
int numThreads = 0;
static int keysOpen;
static int stop = 0;
static int done = 0;
struct IndvThread *workers = NULL;


/*
	* Outputs the thread number of the thread whose working
*/
/*void working(const int &workerNum){
    pthread_mutex_lock(&printer);
		int workTime = rand() % 10 + 1;
    	cout << "Thread " << workerNum << " is working for "<< workTime << "..." << endl;
    pthread_mutex_unlock(&printer);
    sleep(workTime);
}*/
/*
	* Outputs the thread number of the thread whose waiting
*/
/*void waiting(const int &workerNum){
    pthread_mutex_lock(&printer);
		cout << "Thread " << workerNum << " is waiting..." << endl;
    pthread_mutex_unlock(&printer);
}*/

void checkAllDone(struct IndvThread workers[])
{
    for (int i = 0; i < numThreads; i++) {
        if (workers[i].action != "done") {
            done = 0;
        }
        else {
            done = 1;
        }
    }
}

/*
    * Prints the status of all threads
 */
void* printAll(void* num)
{
	int numWorkers = *(int*)num;
	if(workers != NULL){
		while(true){
            checkAllDone(workers);
    		sem_getvalue(&keyHolder, &keysOpen);
		    pthread_mutex_lock(&printer);
				cout << "Keys open: " << keysOpen << endl;
		    pthread_mutex_unlock(&printer);
			for(int i = 0; i < numWorkers; i++){
		    	pthread_mutex_lock(&printer);
					if(workers[i].action == "working"){
						cout << "Thread " << i << " is working for "
                             << workers[i].workWaitTime << " seconds..." << endl;
                        workers[i].workWaitTime--;
                        if (workers[i].workWaitTime == 0) {
                            workers[i].action = "done";
                        }
					}
					else{
						cout << "Thread " << i << " is " << workers[i].action
                             << "..." << endl;
					}
		    	pthread_mutex_unlock(&printer);
			}
			cout << endl;
            if (done == 1) {
                break;
            }
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
void* begin(void *worker)
{
    struct IndvThread* curWorker = (struct IndvThread*)worker;
	while(true){
    	sem_getvalue(&keyHolder, &keysOpen);			
        if(keysOpen == 0) {
            stop = 1;
        }
        
		// /* see how many keys are available */
		if(stop){
			pthread_mutex_lock(&needAllKeys);					
				pthread_cond_wait(&signalAllKeys, &needAllKeys);
                curWorker->action = "waiting";
			pthread_mutex_unlock(&needAllKeys);	
		}

        else {
            // The process is now working //
            sem_wait(&keyHolder);
                curWorker->action = "working";
                curWorker->workWaitTime = rand() % 4+ 1;
                sleep(curWorker->workWaitTime);
                curWorker->action = "waiting";
            sem_post(&keyHolder);
            //curWorker->action = "done"; // the process has now completed its work

            sem_getvalue(&keyHolder, &keysOpen);
            if(keysOpen == MAX_PROCESSES){
                pthread_cond_signal(&signalAllKeys);
                pthread_cond_signal(&signalAllKeys);
                pthread_cond_signal(&signalAllKeys);
                stop = 0;
            }
        }
	}
    return NULL;
}

/*
	* Set up the workers
*/
void initWorkers(struct IndvThread workers[], const int &numThreads)
{
	for(int i = 0; i < numThreads; i++){
		workers[i].threadNum = i;
		workers[i].action = "waiting";
	}
}
/*
	* Sets up the semaphore an mutexes
*/
void initLocks()
{
	sem_init(&keyHolder, 0, 3);
    pthread_mutex_init(&printer, NULL);
    pthread_mutex_init(&needAllKeys, NULL);
	pthread_cond_init(&signalAllKeys, NULL);
}

/*
	* Frees all allocated memory
*/
void freeMemory(pthread_t threads[], struct IndvThread workers[])
{
	if(threads != NULL)
		delete [] threads;
	if(workers != NULL)		
		delete [] workers;
}

/*
	* Joins all running threads
*/
void joinThreads(pthread_t threads[], const int &numThreads)
{
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
int main(int argc, char *argv[])
{
    srand( time(NULL) );

	if(argc != 2){
		cout << "Wrong Format" << endl; 
		cout << "Correct Format: concurrency <num threads>" << endl;
	}
	else{
		numThreads = atoi(argv[1]);
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
                pthread_mutex_unlock(&printer);
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
