/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*	Search Func:
*		- If a deleter thread isn't working
*			○ Lock deleters out
*			○ Then search (aka: print list)
*	Insert Func:
*		- If a deleter isn't working
*			○ Lock deleters out (which also locks inserters)
*			○ Then insert
*	Delete Func:
*		- If a inserter thread isn't working
*			○ Lock out inserters (which also locks deleters)
*			○ If a searcher thread isn't working
*				+ Lock out searchers
*				+ Then delete
*
*	* The function pthread_mutex_trylock(&mutex) returns 0 
*		if mutex is acquired. Otherwise, an error number is returned 
*		to indicate the error.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::list;

struct IndvThread{
	int threadNum;
	string threadType;
};

pthread_mutex_t printer;
pthread_mutex_t searchLock;
pthread_mutex_t insertLock;
pthread_mutex_t deleteLock;

struct IndvThread* searchers = NULL;
struct IndvThread* inserters = NULL;
struct IndvThread* deleters = NULL;

pthread_t *searchThreads = NULL;
pthread_t *insertThreads = NULL;
pthread_t *deleteThreads = NULL;
pthread_t printerThread;

list<int> lst; /* The shared resource */



/**********************************************************************
					  Main Functions Per Thread Type 
**********************************************************************/

/*
	* The Main function for searcher threads
*/
void* searchFunc(void* worker){
    struct IndvThread* curSearcher = (struct IndvThread *)worker;

	while(true){
		if(lst.size() < 15){
			/*if a deleter hasnt locked this lock, then search */ 
			if(pthread_mutex_trylock(&searchLock)){
				/*print entire list*/
				pthread_mutex_lock(&printer);
					cout << curSearcher->threadType << " " << curSearcher->threadNum << ": ";
					for(list<int>::iterator it = lst.begin(); it != lst.end(); it++){
						cout << *it << " ";
					}
					cout << endl << endl;
				pthread_mutex_unlock(&printer);
				pthread_mutex_unlock(&searchLock);
				sleep(1);
			} 
		}
	}
    return NULL;
}
/*
	* The Main function for inerter threads
*/
void* insertFunc(void* worker){
    struct IndvThread* curInserter = (struct IndvThread *)worker;

	while(true){
        if(lst.size() < 15){
			/*grabbing insertLock stops inserters and deleters*/
			if(pthread_mutex_trylock(&insertLock)){
				int val = rand() % 13 + 1;
				pthread_mutex_lock(&printer);
					cout << curInserter->threadType << " " << curInserter->threadNum
						<< " is inserting: " << val << endl;
				pthread_mutex_unlock(&printer);
				lst.push_back(val);
				pthread_mutex_unlock(&insertLock);
				sleep(1);
			}
		}
	}
    return NULL;
}
/*
	* The Main function for deleter threads
	* Note there is no deleteLock used here,
		That is because the delete threads will already get queued up 
		on the pthread_mutex_trylock(&insertLock)
*/
void* deleteFunc(void* worker){
    struct IndvThread* curDeleter= (struct IndvThread *)worker;

	while(true){
			/*If an inserter is running, don't delete, wait till you can claim lock*/
			pthread_mutex_lock(&insertLock);
				/*If a searcher is running, don't delete, wait till you can claim lock*/
				pthread_mutex_lock(&searchLock);
					if(lst.size() > 1){
						int front = *lst.begin();
						lst.pop_front();
						pthread_mutex_lock(&printer);
							cout << curDeleter->threadType << " " << curDeleter->threadNum
								<< " deleted: " << front << endl;
						pthread_mutex_unlock(&printer);
					}
				pthread_mutex_unlock(&searchLock);
			pthread_mutex_unlock(&insertLock);
			sleep(1);
	}
    return NULL;
}


/**********************************************************************
					  Program Set Up 
**********************************************************************/

/*
	* Set up the workers
*/
void nameWorkers(struct IndvThread workers[], const int &numThreads, const string &type){
	for(int i = 0; i < numThreads; i++){
		workers[i].threadNum = i;
		workers[i].threadType = type;
	}
}

void initWorkers(const int &numThreads){
	searchers = new struct IndvThread[numThreads];
	inserters = new struct IndvThread[numThreads];
	deleters = new struct IndvThread[numThreads];

	searchThreads = new pthread_t[numThreads];
	insertThreads = new pthread_t[numThreads];
	deleteThreads = new pthread_t[numThreads];

	nameWorkers(searchers, numThreads, string("searcher"));
	nameWorkers(inserters, numThreads, string("inserter"));
	nameWorkers(deleters,  numThreads, string("deleter"));
}
/*
	* Sets up the semaphore an mutexes
*/
void initLocks(){
    pthread_mutex_init(&searchLock, NULL);
    pthread_mutex_init(&insertLock, NULL);
    pthread_mutex_init(&deleteLock, NULL);
    pthread_mutex_init(&printer, NULL);
}


/**********************************************************************
					  Program Clean Up 
**********************************************************************/

/*
	* Joins all running threads
*/
void joinThreads(pthread_t threads[], const int &numThreads){
	for(int i = 0; i < numThreads; i++){
		pthread_join(threads[i], NULL);
	}
}
/*
	* Join up all threads
*/
void joinAll(const int &numThreads){
	joinThreads(insertThreads, numThreads);
	joinThreads(searchThreads, numThreads);
	joinThreads(deleteThreads, numThreads);
	pthread_join(printerThread,NULL);
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
	* Free all allocated memory
*/
void freeAll(){
	freeMemory(insertThreads, inserters);
	freeMemory(searchThreads, searchers);
	freeMemory(deleteThreads, deleters);
}

void startThreads(pthread_t threads[], struct IndvThread workers[], 
					const int &numThreads, void *(funcPtr)(void *))
{
	
	for(int i = 0; i < numThreads; i++){
		if( pthread_create(&threads[i], NULL, (funcPtr), (void*)&workers[i]) < 0 ){
			pthread_mutex_lock(&printer);
				cout << "Error: "
					<< workers[i].threadType << " "
					<< workers[i].threadNum
					<< " thread could not be initialised ... exiting now"
					<< endl;
			pthread_mutex_unlock(&printer);
			freeAll();	
			exit(EXIT_FAILURE);
		}
	}
}


/**********************************************************************
					  Program Main Function 
**********************************************************************/

/*
	* Main function
*/
int main(int argc, char **argv){
    
    srand( time(NULL) );

	if(argc != 2){
		cout << "Wrong Format" << endl; 
		cout << "Correct Format: part2 <num threads per type>" << endl;
	}
	else{
		int numThreads = atoi(argv[1]);
		if(numThreads < 1){
			cout << "Please allow at least one thread per type" << endl;
		}
		else{
			/* Inititlize Lock Constructs */
    		initLocks();
            
			/* Set up structures */
			initWorkers(numThreads);

			/* Start threads */ 
			startThreads(insertThreads, inserters, numThreads, &insertFunc);
			startThreads(searchThreads, searchers, numThreads, &searchFunc);
			startThreads(deleteThreads, deleters, numThreads, &deleteFunc);
			
			/* Clean everything up */
			joinAll(numThreads);
			freeAll();
		}
	}

	return 0;
}
