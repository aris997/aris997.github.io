#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>



int main () {
	register int i;
	clock_t begin = clock();
	char *file, *main_file;
	main_file =malloc(1000*49);
	memset(main_file,0x00,1000*49);
	file=main_file;

	srand(time(0));
	srand48(time(0));
	
	int Q;
	double k, *memo, M;

	for (Q=0; Q<TOP; Q++) {
		clock_t start = clock();

		M=0.;
		memo = (double *) calloc(PAP, sizeof(double));
		if (memo == NULL) { printf("ERROR CALLOC memo"); exit(-1); }

		for (i=0; i<PAP; i++) {
			k = rand()/(double)RAND_MAX;
			memo[i]=k;
			M += k;
		}

		M/=(double)PAP;
		clock_t partial = clock();
		sprintf(file, "%1.6lf %1.6lf ", M, (double)(partial-start)/(double)CLOCKS_PER_SEC);
		file+=strlen(file);

		/**SECONDO ALGORITMO RAND48**/
		M=0.;
		memo = (double *) calloc(PAP, sizeof(double));
		if (memo == NULL) { printf("ERROR CALLOC memo2"); exit(-1); }

		for (i=0; i<PAP; i++) {
			k = lrand48()/(double)RAND_MAX;
			memo[i]=k;
			M += k;
		}

		M/=(double)PAP;
		clock_t stop = clock();
		sprintf(file, "%1.6lf %1.6lf\n", M, (double)(stop-partial)/(double)CLOCKS_PER_SEC);
		file+=strlen(file);
	}

	FILE *out;
	out = fopen("data.dat", "w");
	fprintf(out, "%s", main_file);
	fclose(out);
	free(memo);
	free(main_file);
	clock_t end = clock();
	printf("execution time :%lf\n", (double)(end-begin)/(double)CLOCKS_PER_SEC);
	system("gnuplot < gnu.gp");
	exit(0);
}
