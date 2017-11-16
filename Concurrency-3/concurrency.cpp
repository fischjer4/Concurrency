/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

using std::cout;
using std::endl;
using std::string;


struct IndvThread{
	int threadNum;
	void* action;
};


/*
	* Outputs the thread number of the thread whose working
*/
void working(const int &workerNum){
	cout << workerNum << " is working" << endl;
}
/*
	* Outputs the thread number of the thread whose waiting
*/
void waiting(const int &workerNum){
	cout << workerNum << " is waiting" << endl;
}
/*
	* The threads base function
	* Decides whether the thread can work yet or not
*/
void begin(void* worker){
		
}
/*
	* Set up the workers
*/
void initWorkers(struct IndvThread workers[], const int &numThreads){
	for(int i = 0; i < numThreads; i++){
		workers[i].threadNum = i;
		workers[i].action = waiting;
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
	if(argc != 2){
		cout << "Wrong Format" << endl; 
		cout << "Correct Format: concurrency <num threads>" << endl;
	}
	else{
		srand(time(NULL));
		int numThreads = atoi(argv[1]);
		if(numThreads < 1){
			cout << "Please choose at least one thread" << endl;
		}
		else{
			/* Set up structures */
			struct IndvThread* workers = new struct IndvThread[numThreads];
			pthread_t* threads = new pthread_t[numThreads];
			initWorkers(workers, numThreads);
			/* Start threads */ 
			for(int i = 0; i < numThreads; i++){
				if( pthread_create(&threads[i], NULL, begin, (void*) &workers[i]) ){
					cout << "Error: "
						<< workers[i].threadNum
						<< " thread could not be initialised ... exiting now"
						<< endl;
				}
			}
			cleanUp(threads, workers, numThreads);
		}
	}

	return 0;
}
