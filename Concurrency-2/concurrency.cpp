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
	string name;
	string action;
};
struct Philosopher allPhilos[5];
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
	* Prints out that the philosopher is eating
*/
void eating(struct Philosopher* curPhilo, int timeEat){
	curPhilo->action = "\\ /";
	cout << curPhilo->name << " is eating for " << timeEat << " seconds" << endl;
	printTable();
	sleep(timeEat);
}
/*
	* Prints out that the philosopher is thinking
*/
void thinking(struct Philosopher* curPhilo, int timeThink){
	curPhilo->action = "?";
	cout << curPhilo->name << " is thinking for " << timeThink << " seconds" << endl;
	printTable();
	sleep(timeThink);
}
/*
	* The "sitting at the table" ... the dining function
*/
void* begin(void* philo){
	pthread_mutex_lock(&chopsticks[0]);
	
	struct Philosopher* curPhilo = (struct Philosopher*)philo;
		thinking(curPhilo, rand_num(1, 20));			

		eating(curPhilo, rand_num(1, 9));
		
		thinking(curPhilo, rand_num(1, 20));		
	pthread_mutex_unlock(&chopsticks[0]);
	pthread_exit(NULL);
}
/*
	* Initializing the mutexes
*/
void initChopsticks(){
	for(int i = 0; i < 5; i++){
		 pthread_mutex_init(&chopsticks[i], NULL);
	}
}
/*
	* Initilizes the five philosophers
*/
void initPhilos(string philoNames[]){
	for(int i = 0; i < 5; i++){
		allPhilos[i].name = philoNames[i];
		allPhilos[i].action = "?";
	}
}

/*
	* Main function
*/
int main(int argc, char *argv[]){
	srand(time(NULL));	
	string philoNames[] = {"Aristotle", "Plato", "Locke", "Socrates", "Marx"};
	pthread_t threads[5];
	initPhilos(philoNames);
	initChopsticks();
	for(int i = 0 ; i < 5; i++){
		if(pthread_create(&threads[i], NULL, begin, (void*) &allPhilos[i])){
			cout << "Error: " 
				 << allPhilos[i].name 
				 << " thread could not be initialised ... exiting now" 
				 << endl;
		}
	}
	
	for(int i = 0; i < 5; i++){
		pthread_join(threads[i], NULL);
	}
	for(int i = 0; i < 5; i++){
		pthread_mutex_destroy(&chopsticks[i]);
	}

	return 0;
}