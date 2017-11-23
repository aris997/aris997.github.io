#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323

//Rodari Riva
//begin 5th october 2017	- v.0.3.2

struct vector{
	double a;
	double va;
	double t;
};

struct vector cromer(struct vector, double, double);
struct vector copy(struct vector, double);

double acceleration(struct vector, double, double, double, double, double);
double energy(struct vector, double);

void period(struct vector, struct vector, double);

int main(int argc, char *argv[]) {

	int k;
	double omega2, gamma, f0, omegaf, dt, tmax, e, e0, phi;
	long int i, steps;
	char filename[20];

	struct vector state;
	struct vector last;

	FILE *input;
	FILE *output;

	input = fopen("input.dat", "r");
	printf("Cromer\n");

	for (k=0; k<2; k++) {

	  fscanf(input, "%lf %lf %lf %lf %lf %lf %lf %lf", &state.a, &state.va, &omega2, &dt, &tmax, &gamma, &f0, &omegaf);

	  e0 = energy(state, omega2);

	  steps = (long int)(tmax/dt);

	  sprintf(filename, "p_cromer%d.dat", k+1);
	  output = fopen(filename, "w");
	  
	  fprintf(output, "#t, \ta_i, \tva_i, \tE(t)/E0 - 1\n");
	  fprintf(output, "%lf %lf %lf %lf\n", 0., state.a, state.va, 0.);


	  for (i=0; i<steps; i++) {

	  	last = copy(state, dt * i);

	  	phi = acceleration(state, omega2, gamma, f0, omegaf, dt * (i+1));
	  	state = cromer(state, dt, phi);
	  	
	  	e = energy(state, omega2);
	  	
	  	fprintf(output, "%lf %lf %lf %lf\n", dt*(i+1), state.a, state.va, e/e0 - 1.);
	  	
	  	period(last, state, omega2);
	  }

	  fclose(output);
	}
	fclose(input);

	exit(0);
}

struct vector cromer(struct vector old, double dt, double phi) {
	
	struct vector new;
	
	new.va = old.va + phi * dt;
	new.a = old.a + new.va * dt;

	return new;
  
}


double energy(struct vector state, double omega2) {

	return 0.5 * state.va * state.va + omega2 * (1 - cos(state.a));
}


double acceleration(struct vector state, double omega2, double gamma, double f0, double omegaf, double t) {

  return - omega2 * sin(state.a) - gamma * state.va + f0 * cos(omegaf * t);
  
}


void period(struct vector old, struct vector new, double omega2) {

	double T, Ta;

	Ta = 2*PI/sqrt(omega2);

	if (old.a + new.a < 0 ) {
  		
  		T = old.t + old.a * (old.t - new.t) / (new.a - old.a);

  		printf("%lf %lf %lf %lf %.10lf %.10lf\n", old.a, new.a, new.t, old.t, T, T/Ta - 1);
  		
  		if (T/Ta - 1 > 0.1 ) {

  			printf("STOP: errore del periodo sopra il 10%% %lf \n", T/Ta - 1);
  			exit(0);

  		}
  	}
}


struct vector copy(struct vector state, double t) {
	
	struct vector new;

	new.a = state.a;
	new.va = state.va;
	new.t = t;

	return new;
}