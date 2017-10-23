/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*	Game Plan
*		• Let the chopsticks be an array of semaphores
*		• Initialize all Semaphores to 1 (note this is technically 
*			a Mutex) (the 1 means the chopstick is available)
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
using namespace std;

/*
	* Defining philosopher Names for output
*/
#define PHIL_1 "Aristotle"
#define PHIL_2 "Plato"
#define PHIL_3 "Locke"
#define PHIL_4 "Socrates"
#define PHIL_5 "Marx"

struct Philosopher{
	string name;
	void (*action)(string);
	int hasBoothForks;
};


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
void eating(string name){
	cout << name << " is eating " << endl;
}
/*
	* Prints out that the philosopher is thinking
*/
void thinking(string name){
	cout << name << " is thinking " << endl;
}

void* begin(void* philo){
	struct Philosopher* curPhilo = (struct Philosopher*)philo;
	curPhilo->action(curPhilo->name);
	pthread_exit(NULL);
}


/*
	* Initilizes the five philosophers
*/
void initPhilos(struct Philosopher allPhilos[]){
	/* First Philosopher */
	allPhilos[0].name = PHIL_1;
	allPhilos[0].action = thinking;
	allPhilos[0].hasBoothForks = 0;
	/* Second Philosopher */
	allPhilos[1].name = PHIL_2;
	allPhilos[1].action = thinking;
	allPhilos[1].hasBoothForks = 0;
	/* Third Philosopher */
	allPhilos[2].name = PHIL_3;
	allPhilos[2].action = thinking;
	allPhilos[2].hasBoothForks = 0;
	/* Fourth Philosopher */
	allPhilos[3].name = PHIL_4;
	allPhilos[3].action = thinking;
	allPhilos[3].hasBoothForks = 0;
	/* Fifth Philosopher */
	allPhilos[4].name = PHIL_5;
	allPhilos[4].action = thinking;
	allPhilos[4].hasBoothForks = 0;
}

/*
	* Main function
*/
int main(int argc, char *argv[]){
	srand(time(NULL));	
	struct Philosopher allPhilos[5];
	pthread_t threads[5];
	initPhilos(allPhilos);
	for(int i = 0 ; i < 5; i++){
		if(pthread_create(&threads[i], NULL, begin, (void*) &allPhilos[i])){
			cout << "Error: " 
				 << allPhilos[i].name 
				 << " thread could not be initialised ... exiting now" 
				 << endl;
		}
	}

	pthread_exit(NULL);	

	return 0;
}