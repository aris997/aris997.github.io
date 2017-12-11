#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define BOOKS 1
#define STORIES 1
#define STEPS 1000
#define GAP 10

//RODARI-RIVA       labfc409      A.A.2017-2018         v0.1.0

/*
    CAMMINO ALEATORIO IN AMBIENTE ALEATORIO
  Su di una retta un viandante ubriaco si muove tra delle trappole che lo tengono occupato per un
  certo periodo di tempo ogni volta. C'è una probabilità definita a priori per ogni BOOK, esso 
  raccoglie ogni storia in cui il viandante si perde nelle trappole e dura MAX passi.
  Il sistema con cui riproduciamo le trappole è un ingegnoso metodo per inserire in modo alternato
  le celle pari con quelle dispare, seguendo il criterio definito da una certa funzione.

*/

typedef unsigned long long int ullint;
typedef long int lint;

void error(int);

int main(){

  register ullint i;
  srand(time(NULL));

  double *envr;
  envr = (double *) malloc( (2*STEPS + 1) * sizeof(double)); //Si inizializza un'array disparo.
  if(envr == NULL) error(100);

  double r;

  lint book;
  lint story;
  lint step;

  lint p;

  double INV_RANDMAX = 1./(double)RAND_MAX;

  for(book = 0; book<BOOKS; book++){

    story = 0;

    envr = (double *) calloc( (2*STEPS + 1), sizeof(double));
    if(envr == NULL) error(100);

    for(i=0; i<(2*STEPS + 1); i++){

      r = (double)rand()*INV_RANDMAX;
      envr[i] = r;


    }


    for(story = 0; story<STORIES; story++){

      p = 0;

      char filename[12];
      sprintf(filename, "story%02ld.dat", story+1); //Creo il file con sistema automatico di denominazione
      FILE *output;
      output = fopen(filename, "w");
      if(output == NULL) error(99);

      fprintf(output, "#step position\n0 0\n");


      for(step = 0; step<STEPS; step++){


        r = rand()*INV_RANDMAX;

        if ( p > 0){
          if(r < envr[2*p-1]) p++;
          else if(r > envr[2*p-1]) p--;
        }
        
        else if(p < 0){
          if(r < envr[-2*p]) p--;
          else if(r > envr[-2*p]) p--;
        }
        
        else if(p == 0){
          if(r < envr[0]) p++;
          else if(r > envr[0]) p--;
        }

        fprintf(output, "%ld %ld\n", step + 1, p);


      }

      fclose(output);

    }

  }

  free(envr);


  exit(0);
}

void error(int n){
  if(n == 100){
    printf("ERROR! ALLOCAZIONE MEMORIA NON AVVENUTA\n");
  }
  else if(n == 99){
    printf("ERROR! FILE NON APERTO CORRETTAMENTE!\n");
  }
  exit(-1);
}