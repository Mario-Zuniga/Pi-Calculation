/* Libraries */
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* Defines */
#define NTHREADS 2
#define NSTEPS 1000000000

/* Global variables */
double step;
pthread_mutex_t candado;	/* Needed for the mutex */
double globalpi = 0.0;

/* Thread function */
void *threadFunc(void *args) {
	/* Value sended when created */
	int threadNUM = *((int *) args);

	/* We get the beggining and ending of the values for the cycle */
	int rinic=threadNUM*(NSTEPS/NTHREADS);
	int rfin=(threadNUM+1)*(NSTEPS/NTHREADS);

	/* Local variables */
	int i;
	double x=0;
	double localpi=0.0;
	
	/* Cycle will calculate the value of pi, this will be done by 2 threads */
	for(i = rinic; i < rfin; i++) {
		x = (i + .5)*step;
		localpi = localpi + 4.0/(1.+ x*x);
	}

	/* We need to lock the other thread before adding the result to the global variable */
	pthread_mutex_lock(&candado);
	globalpi = globalpi + localpi;

	/* Now that it has been added, we unlock the mutex */
	pthread_mutex_unlock(&candado);
}

/* Main functio */
int main(int argc, char* argv[]) {

	/* The arguments that will be sent to the threads */
	int args[NTHREADS];

	/* The threads need an ID before using them */
	pthread_t threadID[NTHREADS];
	
	long long start_ts;
	long long stop_ts;
	float elapsed_time;
	long lElapsedTime;
	struct timeval ts;
	int i;

	/* We start to get the time before calculating pi */
	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec * 1000000 + ts.tv_usec; 
	step = 1./(double)NSTEPS;

	/* We initiate the mutex before using it */
	pthread_mutex_init(&candado,NULL);

	/* The threads are created and execute threadFunc */
	for(i = 0; i < NTHREADS; i++) {
		args[i] = i;
		pthread_create(&threadID[i], NULL, threadFunc, (void *) &args[i]);
	}
	
	for(i = 0; i < NTHREADS; i++)
		pthread_join(threadID[i], NULL);
	
	/* Now that we have the final value of threadFunc, we can get pi */
	globalpi = globalpi * step;

	/* We get the time when we get pi, and with that, we can calulate the total
	   time it took */
	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo final
	elapsed_time = (float) (stop_ts - start_ts)/1000000.0;

	/* We print the results */
	printf("El valor de PI es %1.12f\n",globalpi);
	printf("Tiempo = %2.2f segundos\n",elapsed_time);
	return 0;
}