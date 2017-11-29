#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//RODARI RIVA
//28 settembre 2017   -   v.1.0.1

//INPUT.DAT

//EULERO
//EULERO_CROMER
//MEZZOPASSO


struct vector{
  double x;
  double v;
};

struct vector eulero(struct vector, double, double);
struct vector cromer(struct vector, double, double);
double energy(struct vector, double);

int main(int argc, char *argv[]){

  // DICHIARAZIONE VARIABILI
  
  long int steps;
  int i, j, k;
  double omega2, dt, tmax, e, e0;
  struct vector state;
  
  char filename[50];
  
  FILE *input;
  FILE *output;

  sprintf(filename, "Kappa123");
  puts(filename);

  input = fopen("input.dat", "r");

  for(i=0;i<2;i++){ // i < numero di righe di stati iniziali
    
    fscanf(input, "%lf %lf %lf %lf %lf", &state.x, &state.v, &omega2, &dt, &tmax);

    steps = (long int)tmax/dt;

    printf("%.10lf %.10lf %.10lf %.10lf %.10lf %ld\n", state.x, state.v, omega2, dt, tmax, steps);

    sprintf(filename, "output%.0lf.dat", state.x);

    output = fopen(filename, "w");
    fclose(output);
    
  }

  fclose(input);
  
  exit(0);
}


// Implementazione Algoritmi di Integrazione

struct vector eulero(struct vector old, double dt, double omega2){    //Eulero
  
  struct vector new;
  
  new.x = old.x + old.v * dt;
  new.v = old.v - omega2 * old.x *dt;

  return new;
  
}



struct vector cromer(struct vector old, double dt, double omega2){    //Eulero-Cromer

  struct vector new;
  
  new.v = old.v - omega2 * old.x * dt;
  new.x = old.x + new.v * dt;
  
  return new;
  
}



double energy(struct vector phase, double omega2) {            //Funzione Energia Meccanica

  return 0.5*phase.v*phase.v + 0.5*omega2*phase.x*phase.x;

}


struct vector puntocentrale(struct vector old, double dt, double omega2) {

  struct vector new;

  new.v = old.v - omega2*old.x*dt;
  new.x = old.x + (old.v + new.v)*dt/2;

  return new;
}



/*
struct vector leapfrog(struct vector old, double dt, double omega2, long int steps) {

  static int 
  if(steps==0) {}
  else {

  }

}
*/