#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//RODARI RIVA
//28 settembre 2017   -   v.1.1.2

#define INPUT_ROWS 1

typedef struct vector{
  double x;
  double v;
} vector;

struct vector eulero(vector, double, double);
struct vector cromer(vector, double, double);
double energy(vector, double);

void errori(int);

int main(int argc, char *argv[]){
  register unsigned long long int i;
  // DICHIARAZIONE VARIABILI
  clock_t start = clock();

  long int steps;
  int ii;
  double omega2, dt, tmax, e, e0;
  vector state;
  
  int select=0; //here you can set the default for the choice of the algorithm

  char filename[50];
  
  FILE *input;
  FILE *output;
  FILE *output_energy;

  struct vector (*algorithm)(vector, double, double); //defining the pointer to function

  input = fopen("input.dat", "r");
  if (input == NULL) {errori(404); exit(-1);} //if input dows not exist it's a segmentation fault
  output_energy = fopen("energy.dat", "w");
  
  printf("#Choose the algorithm to process: (0)Eulero, (1)Eulero-Cromer (default is 0)\n");
  do {
    scanf("%d", &select);
  } while (select < 0 || select > 1);

  if (select == 1) algorithm = cromer;
  else algorithm = eulero;


  for(ii=0; ii<INPUT_ROWS; ii++){ // i < numero di righe di stati iniziali
    
    fscanf(input, "%lf %lf %lf %lf %lf", &state.x, &state.v, &omega2, &dt, &tmax);

    steps = (long int)tmax/dt;
    e0 = energy(state, omega2);

    //printf("%.10lf %.10lf %.10lf %.10lf %.10lf %ld\n", state.x, state.v, omega2, dt, tmax, steps);


    sprintf(filename, "dat%d.dat", ii+1); // Da' un nome al file di output e lo apre in scrittura
    output = fopen(filename, "w");

    fprintf(output, "#t\tx_calc\tv_calc\tE(t)/E0 - 1\n");
    fprintf(output, "%.10lf\t %.10lf\t %.10lf\t %.10lf\n", 0., state.x, state.v, 0.);
    
    for (i=0; i<steps; i++){ //il ciclo principale (a lui è riservata la variabile "i" definita come registro %llu)
      
      state = algorithm(state, dt, omega2);
      e = energy(state, omega2);

      if (dt*(i+1) == 2.5) fprintf(output_energy, "%lf %.5lf\n", dt, e/e0 - 1.);
      
      fprintf(output, "%.10lf\t %.10lf\t %.10lf\t %.10lf\n", dt*(i+1), state.x, state.v, e/e0 - 1.);
    }
    
    fclose(output);
  
  }

  fclose(output_energy);
  fclose(input);

  clock_t end = clock();
  printf("Execution time; %lf\n", (double)(end-start)/(double)CLOCKS_PER_SEC);

  exit(0);
}


// Implementazione Algoritmi di Integrazione

struct vector eulero(vector old, double dt, double omega2){    //Eulero
  
  vector new;
  
  new.x = old.x + old.v * dt;
  new.v = old.v - omega2 * old.x *dt;

  return new;
  
}



struct vector cromer(vector old, double dt, double omega2){    //Eulero-Cromer

  vector new;
  
  new.v = old.v - omega2 * old.x * dt;
  new.x = old.x + new.v * dt;

  return new;
  
}



double energy(vector phase, double omega2) {            //Funzione Energia Meccanica

  return 0.5*phase.v*phase.v + 0.5*omega2*phase.x*phase.x;

}


void errori(int n){
  if (n==404){
    printf("ERROR 404\nINPUT FILE NOT FOUND!!!\nCreate an input file faster than light,\nPlease use the following order for starting point:\nx0 y0 omega2 dt tmax\n");
  }
  if (n==-1){
    printf("ERROR -1\nGENERIC PROBLEM\nCall the guys that gived u this awesome code\n");
  }
}

void gnuplot(int n){

}