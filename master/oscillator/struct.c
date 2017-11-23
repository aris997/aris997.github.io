#include <stdio.h>
#include <stdlib.h>

#define EXIT_SUCCESS	 0
#define EXIT_FAIL 		-9
#define	EULER 			 0
#define	EULER_CROMER	 1

struct phaseSpace {
	double x;
	double v;
};

struct phaseSpace *initHarmonicOscillator(double x0, double v0);
struct phaseSpace *euler(double dt, double omega2, struct phaseSpace *pXAndVold);
struct phaseSpace *eulerCromer(double dt, double omega2, struct phaseSpace *pXAndVold);

double energy(double omega2, struct phaseSpace *pXAndV);
double forceHarmonicOscillator(double omega2, double x);
double potentialEnergyHarmoincOscillator(double omega2, double x);
double myReadDouble(char *printMessage);

long int myReadLong(char * printMessage);



int main(void)	{
	double x0, v0, dt, omega2, totalTime, energy0, energy1;
	long int numberOfSteps, algorithm, i;
	struct phaseSpace xAndV;

	printf("#	Integrazione Oscillatore Armonico\n");
	printf("#v1.1.1, 30 settembre 2005\n");

	totalTime = myReadDouble("Inserire il tempo total di integrazione");
	algorithm = myReadLong("Inserire 0 per Eulero, 1 per Eulero-Cromer");

	dt = myReadDouble("Inserire dt");
	x0 = myReadDouble("Inserire x0");
	v0 = myReadDouble("Inserire v0");
	omega2 = myReadDouble("Inserire omega2");

	numberOfSteps = (long int)(totalTime/dt);
	printf("#	dt = %g tempo totale = %g\n", dt, totalTime);
	printf("#	omega2 = %g numero passi = %ld\n", omega2, numberOfSteps);

	xAndV = *initHarmonicOscillator(x0, v0);
	printf("#	Condizioni iniziali x0 = %g v0 = %g\n", xAndV.x, xAndV.v);

	energy0 = energy(omega2, &xAndV);
	printf("#	L'energia al tempo t=0 vale %g\n", energy0);

	if (algorithm == EULER)	{
		printf("#	Usiamo l'algoritmo di Eulero\n");
		for (i=0; i < numberOfSteps; i++)	{
			xAndV = *euler(dt, omega2, &xAndV);
			energy1 = energy(omega2, &xAndV);
			printf("%g %g %g %g\n", (double)i*dt, xAndV.x, xAndV.v, energy1-energy0);
		}
	}
	else if (algorithm == EULER_CROMER)	{
		printf("#   Usiamo l'algoritmo di Eulero-Cromer\n");
		for (i=0; i < numberOfSteps; i++)	{
			xAndV = *eulerCromer(dt, omega2, &xAndV);
			energy1 = energy(omega2, &xAndV);
			printf("%g %g %g %g\n", (double)i*dt, xAndV.x, xAndV.v, energy1-energy0);
		}
	}
	else {
		printf("#   TUONI e FULMINI: algoritmo n. %ld non previsto. Errore.\n", algorithm);
		exit(EXIT_FAIL);
	}

	energy1 = energy(omega2, &xAndV);
	printf("#   L'energia al tempo t = %ld vale %g\n", numberOfSteps, energy1);
	return EXIT_SUCCESS;
}







struct phaseSpace *initHarmonicOscillator(double x0, double v0)	{
	static struct phaseSpace xAndV;
	xAndV.x = x0;
	xAndV.v = v0;
	return &xAndV;
}

struct phaseSpace *euler(double dt, double omega2, struct phaseSpace *pXAndVold)	{
	static struct phaseSpace xAndVnew;

	xAndVnew.v = (*pXAndVold).v + forceHarmonicOscillator(omega2, (*pXAndVold).x)*dt;
	xAndVnew.x = pXAndVold->x + pXAndVold->v * dt;
	return &xAndVnew;
}

struct phaseSpace *eulerCromer(double dt, double omega2, struct phaseSpace *pXAndVold)	{
	static struct phaseSpace xAndVnew;

	xAndVnew.v = (*pXAndVold).v + forceHarmonicOscillator(omega2, (*pXAndVold).x) * dt;
	xAndVnew.x = pXAndVold->x + pXAndVold->v * dt;
	return &xAndVnew;
}

double energy(double omega2, struct phaseSpace *pXAndV)	{
	double localEnergy;
	localEnergy = (*pXAndV).v * (*pXAndV).v + potentialEnergyHarmoincOscillator(omega2, (*pXAndV).x);
	return localEnergy;
}

double forceHarmonicOscillator(double omega2, double x)	{
	return - omega2*x;
}

double potentialEnergyHarmoincOscillator(double omega2, double x)	{
	return omega2*x*x;
}

double myReadDouble(char * printMessage)	{
	double inputData;

	printf("#	%s\n", printMessage);
	fflush(stdout);
	scanf("%lg", &inputData);

	return inputData;
}

long int myReadLong(char * printMessage)	{
	long int inputData;

	printf("#	%s\n", printMessage);
	fflush(stdout);
	scanf("%ld", &inputData);

	return inputData;
}





