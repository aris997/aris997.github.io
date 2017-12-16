#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define BOOKS 100
#define STORIES 100
#define STEPS 10000

//RODARI-RIVA       labfc409      A.A.2017-2018         v0.1.0

/*
    CAMMINO ALEATORIO IN AMBIENTE ALEATORIO
  Su di una retta un viandante ubriaco si muove tra delle trappole che lo tengono occupato per un
  certo periodo di tempo ogni volta. C'è una probabilità definita a priori per ogni BOOK, esso 
  raccoglie ogni storia in cui il viandante si perde nelle trappole e dura MAX passi.
  Merita attenzione il metodo che abbiamo scelto per memorizzare le probabilità ambientali.
  Il sistema con cui riproduciamo le trappole è un ingegnoso metodo per inserire in modo alternato
  le celle pari con quelle dispare, seguendo il criterio definito da un certo sistema di funzioni.
  Il sistema è applicato nel cuore dei cicli

    BOOKS for
      STORIES for
          STEPS for
              -----
              a questo punto del programma tramite degli if, in base al punto in cui si trova il
              viandante (a sinistra o a destra o sullo zero), si sceglie la cella dell'array che 
              contiene le probabilità con i (indice dell'array delle probabilità)

              || i = 2 * p - 1     con p > 0
              || i = - 2 * p       con p < 0
              || i = 0             con p = 0
*/

typedef unsigned long long int ullint;
typedef long int lint;

typedef struct meanbook{
  double book;
  double book2;
}meanbook;



void error(int);

int main(){

  register ullint i;
  srand(time(NULL));

  double *envr;
  envr = (double *) malloc( (2*STEPS + 1) * sizeof(double)); //Si inizializza un'array disparo.
  if(envr == NULL) error(1);

  meanbook *log;
  log = (meanbook*) malloc( (STEPS) * sizeof(meanbook));
  if(log == NULL) error(1);

  meanbook *mean;
  mean = (meanbook*) calloc(STEPS, sizeof(meanbook));
  if(log == NULL) error(1);

  double r;

  lint book, story, step, p;

  double INV_RANDMAX = 1./(double)RAND_MAX;
  double invSTORY = 1./(double)STORIES;
  double invBOOK = 1./(double)BOOKS;


  for(book = 0; book<BOOKS; book++){

    story = 0;

    envr = (double *) calloc( (2*STEPS + 1), sizeof(double));
    if(envr == NULL) error(1);


    //Inizializzo l'ambiente aleatorio con questo ciclo
    //Le medie sulle storie vengono fatte per ogni libro
    for(i=0; i<(2*STEPS + 1); i++){

      r = (double)rand()*INV_RANDMAX;
      envr[i] = r;
      
      #ifdef DEBUG
        fprintf(stderr, "%llu %lf\n", i, r);
      #endif

    }


    log = (meanbook*) calloc(STEPS, sizeof(meanbook));
    if(log == NULL) error(1);
    for(i=0; i<STEPS; i++) {log[i].book = 0.; log[i].book2 = 0.; }



    for(story = 0; story<STORIES; story++){

      p = 0;
      #ifdef RAW_MODE
        char filename[12];
        //Creo il file con sistema automatico di denominazione
        sprintf(filename, "story%02ld.dat", story+1);
        FILE *output;
        output = fopen(filename, "w");
        if(output == NULL) error(2);

        fprintf(output, "#step position\n0 0\n");
      #endif

      for(step = 0; step<STEPS; step++){

        r = (double)rand()*INV_RANDMAX;

        if ( p > 0){
          if(r < envr[2*p-1]) p++;
          else if(r >= envr[2*p-1]) p--;
          else error(3);
        }
        
        else if(p < 0){
          if(r < envr[-2*p]) p++;
          else if(r >= envr[-2*p]) p--;
          else error(3);
        }
        
        else if(p == 0){
          if(r < envr[0]) p++;
          else if(r >= envr[0]) p--;
          else error(3);
        }

        else error(4);

        #ifdef RAW_MODE
          fprintf(output, "%ld %ld\n", step + 1, p);
        #endif
        
        log[step].book += (double)p;
        log[step].book2 += (double)p*p;

      }
      #ifdef RAW_MODE
        fclose(output);
      #endif
    }



    for(step=0; step<STEPS; step++){
      mean[step].book += log[step].book * invSTORY; 
      mean[step].book2 += log[step].book2 * invSTORY;
    }

  }


    for(step=0; step<STEPS; step++){
      mean[step].book *= invBOOK;
      mean[step].book2 *= invBOOK;
      fprintf(stdout, "%ld %ld %lf %lf\n", step, step*step, mean[step].book, mean[step].book2);
    }


  free(envr);
  free(mean);
  free(log);

  exit(0);
}

void error(int n){
  if(n == 1){
    fprintf(stderr, "ERROR! ALLOCAZIONE MEMORIA NON AVVENUTA\n");
    exit(-1);
  }
  else if(n == 2){
    fprintf(stderr, "ERROR! FILE NON APERTO CORRETTAMENTE!\n");
    exit(-1);
  }
  else if(n == 3){
    fprintf(stderr, "ERROR: rand number [0,1] didn't work\n");
    exit(-1);
  }
  else if(n == 4){
    fprintf(stderr, "ERROR: position is out o range\n");
    exit(-1);
  }
  else exit(-1);
}