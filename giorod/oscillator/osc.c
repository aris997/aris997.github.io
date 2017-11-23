#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//RODARI RIVA
//28 settembre 2017   -   v.1.0.2

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
  int i, j;
  double omega2, dt, tmax, e, e0;
  struct vector state;
  
  char filename[50];
  
  FILE *input;
  FILE *output;
  FILE *order;

  input = fopen("input.dat", "r");
  order = fopen("energy.dat", "w");
  
  for(i=0;i<5;i++){ // i < numero di righe di stati iniziali
    
    fscanf(input, "%lf %lf %lf %lf %lf", &state.x, &state.v, &omega2, &dt, &tmax);

    steps = (long int)tmax/dt;
    e0 = energy(state, omega2);

    printf("%.10lf %.10lf %.10lf %.10lf %.10lf %ld\n", state.x, state.v, omega2, dt, tmax, steps);

    sprintf(filename, "cromer%d.dat", i); // Da' un nome al file di output e lo apre in scrittura
    output = fopen(filename, "w");

    fprintf(output, "# Structure: t,\t x_calc,\t v_calc,\t E(t)/E0 - 1\n");
    fprintf(output, "\t\t%.10lf\t %.10lf\t %.10lf\t %.10lf\n", 0., state.x, state.v, 0.);
    
    for (j=0; j<steps; j++){
      
      state = cromer(state, dt, omega2);

      e = energy(state, omega2);

      if (dt*(j+1) == 2.5){
	fprintf(order, "%lf %.5lf\n", dt, e/e0 - 1.);
      }
      
      fprintf(output, "\t\t%.10lf\t %.10lf\t %.10lf\t %.10lf\n", dt*(j+1), state.x, state.v, e/e0 - 1.);
      
    }
    
    fclose(output);
    
  }

  fclose(order);
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
