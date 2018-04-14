/* Libraries */
#include <stdio.h>
#include <sys/time.h>

/* Defines */
#define NSTEPS 1000000000

/* Global variables */
double step;

/* Main function */
int main(int argc, char* argv[]) {
	/* Local variables */
	long long start_ts;
	long long stop_ts;
	float elapsed_time;
	long lElapsedTime;
	struct timeval ts;
	double x, pi=0.0;
	int i;

	/* We start with getting the time when we start to find the value
	   of pi */
	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec * 1000000 + ts.tv_usec; 
	step = 1./(double)NSTEPS;
	
	/* This cycle will get the value of pi */
	for (i = 0; i < NSTEPS; i++) {
		x = (i + .5) * step;
		pi = pi + 4.0/(1. +  x * x);
	}

	/* We get the final value of pi */
	pi = pi*step;

	/* We get the time when we get pi, and with that, we can calulate the total
	   time it took */
	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; 
	elapsed_time = (float) (stop_ts - start_ts)/1000000.0;

	/* We print the results */
	printf("El valor de PI es %1.12f\n",pi);
	printf("Tiempo = %2.2f segundos\n",elapsed_time);
	return 0;
}