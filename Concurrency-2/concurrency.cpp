#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*
	* Defining philosopher Names for output
*/
#define PHIL_1 "Aristotle"
#define PHIL_2 "Plato"
#define PHIL_3 "Locke"
#define PHIL_4 "Socrates"
#define PHIL_5 "Marx"


/*
	* Returns a random integer in the 
	  inclusive range of [inclus_min, inclus_max]
*/
int rand_num(int inclus_min, int inlus_max){
   	return rand() % (inlus_max - inclus_min + 1) + inclus_min;
}



/*
	* Main function
*/
int main(int argc, char *argv[]){
	int i;
	srand(time(NULL));	
	for(i = 0; i < 10; i++){
		printf("%d\n", rand_num(1,13));
	}
	printf("\n***********\n");
	for(i = 0; i < 10; i++){
		printf("%d\n", rand_num(14,25));
	}
	return 0;
}