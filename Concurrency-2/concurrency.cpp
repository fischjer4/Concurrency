/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*	Game Plan
*		• Let the chopsticks be an array of mutexes
*		• When the ith philosopher wants to eat
*			○ Wait for chopstick[i]
*			○ Wait for chopstick[ (i+1) % 5]
*			○ Eat
*			○ When done eating
*				- Signal chopstick[i]
*				- Signal chopstick [ (i+1 ) % 5]
*		• The  ith  Philosopher thinks
*
*	Potential Problem
*		• What happens if each philosopher grabs the chopstick to their right?
*			○ Then they are all in deadlock (or starvation) 
*				because they are all waiting on the left chopstick to eat
*			Solutions:
*				1) Only allow four philosophers to eat at a time
*				2) A philosopher will only start eating if both 
*					chopsticks are available
*				3) Even number philosophers take the chopstick to their 
*					right first and odd number philosophers will take the 
*					chopstick to their left first. Deadlock can't take place here
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;


struct Philosopher{
	int philoNumber;
	string name;
	string action;
};
struct Philosopher allPhilos[5];
pthread_mutex_t printToOut;
pthread_mutex_t chopsticks[5];

/*
	* Prints the five philosophers and their currecnt action
*/
void printTable(){
	cout << endl << "******************************************************************************" << endl;	
	cout << "			" << allPhilos[0].name << " " 
		 << allPhilos[0].action << endl << endl;
	cout << allPhilos[1].name << " " << allPhilos[1].action 
		 << "						" 
		 << allPhilos[2].name << " " << allPhilos[2].action << endl << endl;
	cout <<"	" 
		 << allPhilos[3].name << " " << allPhilos[3].action 
		 << "			" 
		 << allPhilos[4].name << " " << allPhilos[4].action << endl;
	cout << endl << "******************************************************************************" << endl;
}
/*
	* Returns a random integer in the 
	   inclusive range of [inclus_min, inclus_max]
*/
int rand_num(int inclus_min, int inlus_max){
   	return rand() % (inlus_max - inclus_min + 1) + inclus_min;
}
/*
	* Prints Out That The Philosopher Is Eating
*/
void eating(struct Philosopher* curPhilo, int timeEat){
	curPhilo->action = "\\ /";
	pthread_mutex_lock(&printToOut);
	// Locking STDOUT
	cout << curPhilo->name << " is eating for " << timeEat << " seconds" << endl;
	printTable();
	// UNLOCKING STDOUT
	pthread_mutex_unlock(&printToOut);
	sleep(timeEat);
}
/*
	* Prints Out That The Philosopher Is Thinking
*/
void thinking(struct Philosopher* curPhilo, int timeThink){
	curPhilo->action = "?";
	pthread_mutex_lock(&printToOut);
	// Locking STDOUT
	cout << curPhilo->name << " is thinking for " << timeThink << " seconds" << endl;
	printTable();
	// UNLOCKING STDOUT
	pthread_mutex_unlock(&printToOut);
	sleep(timeThink);
}
/*
	* Have Everuyone Sit At The Table (Main Thread Function)
*/
void* begin(void* philo){
	struct Philosopher* curPhilo = (struct Philosopher*)philo;
	pthread_mutex_lock(&chopsticks[curPhilo->philoNumber]);
	// pthread_mutex_lock(&chopsticks[ (curPhilo->philoNumber % 5) ]);
	// Begin Locked Area
	thinking(curPhilo, rand_num(1, 2));			
	eating(curPhilo, rand_num(1, 2));
	// thinking(curPhilo, rand_num(1, 20));		
	// End Locked Area
	// pthread_mutex_lock(&chopsticks[ (curPhilo->philoNumber % 5) ]);	
	pthread_mutex_unlock(&chopsticks[curPhilo->philoNumber]);
	
	pthread_exit(NULL);
}
/*
	* Get The Chopsticks (Initializing the mutexes)
*/
void initChopsticks(){
	for(int i = 0; i < 5; i++){
		 pthread_mutex_init(&chopsticks[i], NULL);
	}
}
/*
	* Invite  the five philosophers (Initializing the mutexes)
*/
void initPhilos(string philoNames[]){
	for(int i = 0; i < 5; i++){
		allPhilos[i].name = philoNames[i];
		allPhilos[i].action = "?";
		allPhilos[i].philoNumber = i;
	}
}

/*
	* Main function
*/
int main(int argc, char *argv[]){
	srand(time(NULL));	
	string philoNames[] = {"Aristotle", "Plato", "Locke", "Socrates", "Marx"};
	pthread_t threads[5];

	/* Invite Philosophers And Get Chopsitcks (Init threads and Mutexes)*/
	pthread_mutex_init(&printToOut, NULL);
	initPhilos(philoNames);
	initChopsticks();
	
	/* Everyone Sit At The Table (Start The Threads) */
	for(int i = 0 ; i < 5; i++){
		if(pthread_create(&threads[i], NULL, begin, (void*) &allPhilos[i])){
			cout << "Error: " 
				 << allPhilos[i].name 
				 << " thread could not be initialised ... exiting now" 
				 << endl;
		}
	}
	/* Say Goodbye And Wash Chopsitcks (Join threads and Clean Up Mutexes)*/
	for(int i = 0; i < 5; i++){
		pthread_join(threads[i], NULL);
	}
	pthread_mutex_destroy(&printToOut);
	for(int i = 0; i < 5; i++){
		pthread_mutex_destroy(&chopsticks[i]);
	}

	return 0;
}