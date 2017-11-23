#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//RODARI RIVA
//28 settembre 2017   -   v.1.0


void eulero(double *x, double *v, double dt, double omega2);
void cromer(double *x, double *v, double dt, double omega2);
double energy(double x, double v, double omega2);                // Energia per unita' di massa

int main(int argc, char *argv[]){
  
  int choice = 2;         //Sistema di selezione dell'algoritmo
  long int i, steps;                                             //Definizione variabili utilizzate
  double x, v, e, e0, omega2, dt, tmax; //, A, phi;                  //Variabili per gli algoritmi e calcolo analitico 
 
  FILE *table;
  
    //Controllo che gli argomenti da linea di comando siano corretti
  
  if(argc != 6){
    printf("Usage: x0 v0 omega2 dt time\n");
    exit(EXIT_FAILURE);
  }
  
    //Assegnazione stato iniziale e costanti.

  x = atof(argv[1]);        //Raccolgo le informazioni da linea di comando con la f atof()
  v = atof(argv[2]);
  omega2 = atof(argv[3]);
  dt = atof(argv[4]);
  tmax = atof(argv[5]);

  e0 = energy(x, v, omega2);
  
  steps = (long int)(tmax/dt);    //calcolo il numero di cicli e controllo con un cast
  

  table = fopen("output.txt", "w");           //Apertura file

  fprintf(table, "# Structure: t,\t x_calc,\t v_calc,\t E(t)/E0 - 1\n");
  fprintf(table, "\t%lf\t %lf\t %lf\t %lf\n", 0., x, v, 0.);     //inserisco le condizioni iniziali nel output
 
  while(choice != 0 && choice != 1){            //Chiedo quale algoritmo utilizzare
    printf("Insert (0) for Euler, (1) for Euler-Cromer: ");
    scanf("%d", &choice);
  }

  if(choice == 0){                //Algoritmo di Eulero
    for (i=0; i<steps; i++){
      eulero(&x, &v, dt, omega2);
      e = energy(x, v, omega2);
      fprintf(table, "\t%lf\t %lf\t %lf\t %lf\n", dt*(i+1), x, v, e/e0 - 1.);   //Non usiamo array, printiamo direttamente su output
    }
  }
  else{
    for (i=0; i<steps; i++){      //Algoritmo di Eulero-Cromer
      cromer(&x, &v, dt, omega2);
      e = energy(x, v, omega2);
      fprintf(table, "\t%lf\t %lf\t %lf\t %lf\n", dt*(i+1), x, v, e/e0 - 1.);
    }
  }

  fclose(table);                  //chiusura del file

  exit(0);
}


//Implementazione Algoritmi di Integrazione

void eulero(double *x, double *v, double dt, double omega2){    //Eulero
  
  double xnew, vnew;      //Definisco variabili temporali
  
  xnew = (*x) + (*v) * dt;
  vnew = (*v) - omega2*(*x)*dt;

  *x = xnew;            //Assegno tramite puntatori i nuovi valori
  *v = vnew;
  
  return;
}

void cromer(double *x, double *v, double dt, double omega2){    //Eulero-Cromer
  
  double xnew, vnew;    //Definisco variabili temporali
  
  vnew = (*v) - omega2*(*x)*dt;
  xnew = (*x) + vnew * dt;

  *x = xnew;            //Assegno tramite puntatori i nuovi valori
  *v = vnew;
  
  return;
}



double energy(double x, double v, double omega2) {            //Funzione Energia Meccanica

  return 0.5*v*v + 0.5*omega2*x*x;

}
