#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//RODARI RIVA
//28 settembre 2017   -   v.2.0.0 //AGGIORNATO AL 30 NOVEMBRE 2017

/********OSCILLATORE ARMONICO********/
//note di sviluppo
//- algoritmi implementati: EULERO, EULERO-CROMER, PUNTO CENTRALE, MEZZO PASSO
//- GNUPLOT AUTOMATICO SE DEFINITO DA GCC (INOLTRE CREA UNO SCRIPT SE NON ESISTE)
//- L'ALGORITMO USATO VIENE SCELTO DALL'UTENTE OGNI VOLTA (IL SISTEMA DI SCELTA PREVEDE PUNTATORI A FUNZIONI)
//- DAL FILE INPUT È POSSIBILE ESEGUIRE PIÙ CASI, RICORDARE DI CAMBIARE IL VALORE
//  DEL DEFINE "INPUT_ROWS" PER LEGGERE PIÙ RIGHE DEL FILE INPUT
//- FUNZIONI DI CONTROLLO FILE, IN CASO DI MANCAZA DEI FILE IL PROGRAMMA SI CHIUDE
//  EVITANDO INUTILI ERRORI

#define INPUT_ROWS 1

typedef unsigned long long int index;

typedef struct vector{
  double x;
  double v;
} vector;

struct vector eulero(vector, double, double, index);
struct vector cromer(vector, double, double, index);
struct vector centerpoint(vector, double, double, index);
struct vector halfstep(vector, double, double, index);

struct vector copy(vector);

double energy(vector, double);

void errori(int);
void gnuplot();


int main(int argc, char *argv[]){
  // DICHIARAZIONE VARIABILI
  register index i;
  clock_t start = clock();

  int ii, select=0; //here you can set the default for the choice of the algorithm

  double omega2, dt, tmax, e, e0;
  unsigned long long int steps;
  vector state;
  
  char filename[50];
  
  FILE *input, *output, *output_energy;

  struct vector (*algorithm)(vector, double, double, index); //defining the pointer to function

  input = fopen("input.dat", "r");
  if (input == NULL) {errori(404); exit(-1);} //if input dows not exist it's a segmentation fault
  output_energy = fopen("energy.dat", "w");
  
  do {
    printf("#Choose the algorithm to process:\n#(0)Eulero, (1)Eulero-Cromer, (2)Central_Point, (3)HalfStep\n");
    scanf("%d", &select);
  } while (select < 0 || select > 3);

  if (select == 1) algorithm = cromer;
  else if (select == 2) algorithm = centerpoint;
  else if (select == 3) algorithm = halfstep;
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
      
      state = algorithm(state, dt, omega2, i);
      e = energy(state, omega2);

      if (dt*(i+1) == 2.5) fprintf(output_energy, "%lf %.5lf\n", dt, e/e0 - 1.);
      
      fprintf(output, "%.10lf\t %.10lf\t %.10lf\t %.10lf\n", dt*(i+1), state.x, state.v, e/e0 - 1.);
    }
    
    fclose(output);  
  }

  fclose(output_energy);
  fclose(input);
  clock_t end = clock();
  printf("Execution time: %lf\n", (double)(end-start)/(double)CLOCKS_PER_SEC);

  #ifdef GNUPLOT
    FILE *gpscript;
    gpscript = fopen("osc.gp", "r");
    if (gpscript == NULL) gnuplot();
    system("gnuplot < osc.gp");
  #endif

  exit(0);
}


// Implementazione Algoritmi di Integrazione

struct vector eulero(vector old, double dt, double omega2, index i){    //Eulero
  
  vector new;
  
  new.x = old.x + old.v * dt;
  new.v = old.v - omega2 * old.x *dt;

  return new;
  
}



struct vector cromer(vector old, double dt, double omega2, index i){    //Eulero-Cromer

  vector new;
  
  new.v = old.v - omega2 * old.x * dt;
  new.x = old.x + new.v * dt;

  return new;
  
}

struct vector centerpoint(vector old, double dt, double omega2, index i){
  
  vector new;

  new.v = old.v - omega2 * old.x * dt;
  new.x = old.x + 0.5 * ( old.v + new.v ) * dt;

  return new;
}

struct vector halfstep(vector old, double dt, double omega2, index i){

  vector new, hew; //hew -> h-ew -> half-new

  if (i == 0) {
    hew.v = old.v - 0.5 * omega2 * old.x * dt;
  }
  else {
    hew = copy(old);
  }

  new.v = hew.v - omega2 * old.x * dt;
  new.x = old.x + new.v * dt;
 
  return new;
}


double energy(vector phase, double omega2) {            //Funzione Energia Meccanica

  return 0.5*phase.v*phase.v + 0.5*omega2*phase.x*phase.x;

}


void errori(int n){
  if (n==404){
    printf("ERROR 404\nINPUT FILE NOT FOUND!!!\nCreate an input file faster than light,\n");
    printf("Please use the following order for starting point:\nx0 y0 omega2 dt tmax\n");
    printf("Hint:\n2. 1. 3. 0.01 50\n");
  }
  if (n==-1){
    printf("ERROR -1\nGENERIC PROBLEM\nCall the guys that gived u this awesome code\n");
  }
}


void gnuplot(){
  FILE *script;
  script = fopen("osc.gp", "w");

  int s;
  for(s=0; s<INPUT_ROWS; s++){
    fprintf(script, "set term x11 %d enhanced persist\nset grid\n", s);
    fprintf(script, "plot \"dat%d.dat\" u 2:3 w l lc rgb \"red\"\n", s+1);
    fprintf(script, "set term x11 %d enhanced persist\n", s+1);
    fprintf(script, "plot \"dat%d.dat\" u 1:3 w l lc rgb \"blue\"\n", s+1);
    fprintf(script, "replot \"dat%d.dat\" u 1:2 w l lc rgb \"green\"\n", s+1);
  }
  fclose(script);
}

struct vector copy(vector a){
  return a;
}