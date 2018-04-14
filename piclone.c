/* Define necesary for the clone */
#define _GNU_SOURCE

/* Libraries */
#include <stdio.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>

/* Defines */
#define NSTEPS 1000000000
#define STACK_SIZE 8192
#define CORES 2

/* Global variables */
double step;
void * stack[2];
double global_x = 0.0;
double global_pi = 0.0;
int final_flag = 0;

/* Function executed by the clones */
int cloneFunc(void *args) {

	/* We recieve a value from main */
	int coreNUM=*((int *) args);

	/* With that value we can set the beggining and end of the cycle */
	int rinic = coreNUM * (NSTEPS / CORES);
	int rfin = (coreNUM + 1) * (NSTEPS / CORES);

	/* Local variables */
	int i;
	double local_x = 0.0;
	double local_pi_x = 0.0;

	/* This cycle will calculate the value of pi, this will be divided in 2 parts, done by the clone 2 
	   different times */
	for(i = rinic; i < rfin; i++) {
		local_x = (i + .5) * step;
		local_pi_x = local_pi_x + 4.0 / (1. + local_x * local_x);
	}
	
	/* The value obtained is added to the global variable of pi */
	global_pi = global_pi + local_pi_x;

	/* The final flag must be updated indicating that the clone finished it's executio */
	final_flag++;
}

/* Main functio */
int main(int argc, char* argv[]) { 

	/* args will send to clone Func values */
	int args[CORES];

	/* Stack is used to assign stack to the clones, this must be different between each one */
	void * stack[2];
	stack[0] = malloc(STACK_SIZE);
	stack[1] = malloc(STACK_SIZE * 8);
	
	/* Local variables */
	long long start_ts;
	long long stop_ts;
	float elapsed_time;
	long lElapsed_time;
	struct timeval ts;
	int i, j, y;
	
	/* We start to get the time before calculating pi */
	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec * 1000000 + ts.tv_usec;	/* Tiempo inicial */
	step = 1./(double)NSTEPS;

	/* args is updated with the values it will send */
	args[0] = 0;
	args[1] = 1;

	/* Clone is executed, each one executes cloneFunc, both will run in pararel with the main
	   code */
	clone(&cloneFunc, (char *)stack[0]+STACK_SIZE, CLONE_FS|CLONE_VM, (void *) &args[0]);
	clone(&cloneFunc, (char *)stack[1]+STACK_SIZE, CLONE_FS|CLONE_VM, (void *) &args[1]);
		
	/* This cycle works with the function to stop the code while the clones finish executing */
	while(final_flag != 2){
	}
	
	/* Now that we have the final value of cloneFunc, we can get pi */
	global_pi = global_pi * step;

	/* We get the time when we get pi, and with that, we can calulate the total
	   time it took */
	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec * 1000000 + ts.tv_usec;		
	elapsed_time = (float) (stop_ts - start_ts) / 1000000.0;

	/* We print the results */
	printf("El valor de pi es: %1.12f\n", global_pi);
	printf("Tiempo usando clone() = %2.2f segundos\n", elapsed_time);
}