/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* Defines */
#define NSTEPS 1000000000
#define NPROCCESS 2
#define SHMSZ     27

/* Global variables */
double step;

/* Main function */
int main(int argc, char* argv[]) {

	/* Local variables */
	int i;
	int pid;
	int status;
	int rinic;
	int rfin;
	long long start_ts;
	long long stop_ts;
	float elapsed_time;
	long lElapsedTime;
	struct timeval ts;
	double x, pi=0.0;
	double shmid;
    
    double *shm, *s;

    /* This is a declaration for the segment */
    key_t key;
    key = 5678;

    /* We start to get the time before calculating pi */
    gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo inicial
	step = 1./(double)NSTEPS;

	/* We need to get the segment named and localized */
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
		/* If the segment couldn't be created, we announce the error and finish */
        perror("shmget");
        exit(1);
    }

    /* We add the segment to the memory space */
     if ((shm = shmat(shmid, NULL, 0)) == (double *) - 1) {
     	/* If it couldn't be added, we announce the error and finish */
        perror("shmat");
        exit(1);
    }

    /* */
    s = shm;
    *s = 0;

    /* We create the son process */
	pid = fork();

	/* We verify is the pid of the son is correct, if it's 0, it was created correctly */		
	if(pid == 0) {

		/* Data for the cycle to begin, this section is reserved for the son */
		rinic = 1 * (NSTEPS / NPROCCESS);
		rfin = 2 * (NSTEPS / NPROCCESS);

		/* Calculation of the first half of pi begins */
		for (i = rinic; i < rfin; i++) {
			x = (i + .5) * step;
			pi = pi + 4.0 / (1. +  x * x);
		}
		*s = *s + pi;

	} else {

		/* Data for the cycle to begin, this section is reserved for the father */
		rinic = 0 * (NSTEPS / NPROCCESS);
		rfin = 1 * (NSTEPS / NPROCCESS);

		/* Calculation of the second half of pi begins */
		for (i = rinic; i < rfin; i++) {
			x = (i + .5) * step;
			pi = pi + 4.0 / (1. + x * x);
			
		}
			

		wait(&status);	
		*s = *s + pi;

	pi = pi*step;
	*s=(*s)*step;

		gettimeofday(&ts, NULL);
		stop_ts = ts.tv_sec * 1000000 + ts.tv_usec; // Tiempo final
		elapsed_time = (float) (stop_ts - start_ts)/1000000.0;
		
		printf("El valor de PI2 es %1.12f\n",*s);
		printf("Tiempo = %2.2f segundos\n",elapsed_time);
		return 0;
	}

}