#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define PI 3.1415926535897932384626433832795

#define LBIN 0.05

typedef unsigned long long int ullint;
typedef long int lint;

void makehist(lint,int,histogram*);
void error(int);

typedef struct histogram{
  double gamma1;
  double gamma2;
}histogram;

int main(int argc, char **argv){

  clock_t begin = clock();

  srand(time(NULL));
  srand48(time(NULL));

  if(sizeof(ullint) < 8){ error(64); }

  if(argc != 3)error(1);

  lint MAX = atoi(argv[1]);
  int BIN = atoi(argv[2]);

  fprintf(stdout,"# Generatore rand() con distribuzione di Box-Muller\n"
          "# %ld realizzazioni distribuite su %d bin.\n", MAX, BIN);

  

  int bin;
  lint i;
  double ran1 = 0, ran2 = 0, gamma, arg1, arg2;
  double INV_RANDMAX = 1./RAND_MAX;


  histogram *hist;
  hist = (histogram *)calloc(MAX, sizeof(histogram));
  if(hist == NULL){error(101);}


  for(i=0; i<MAX; i++){

    ran1 = rand()*INV_RANDMAX;
    ran2 = rand()*INV_RANDMAX;

    arg1 = sqrt( - 2. * log(ran1) );
    arg2 = 2. * PI * ran2;

    gamma = arg1 * cos(arg2);
    hist[i].gamma1 = gamma;
    gamma = arg1 * sin(arg2);
    hist[i].gamma2 = gamma;
  }
//ISTOGRAMMA in outputH - 

  makehist(MAX, BIN, hist);

  clock_t end = clock();
  printf("# Execution time:%lf\n", (double)(end-begin)/CLOCKS_PER_SEC);

  exit(0);
}


void makehist(lint MAX, int BIN, histogram *hist){

  int index;
  lint i;
  histogram NBIN, *bin, min, max;
  min.gamma1 = 0.;
  min.gamma2 = 0.;
  max.gamma1 = 0.;
  max.gamma2 = 0.;

  for(i=0; i<MAX; i++){
    if(min.gamma1 > hist[i].gamma1) min.gamma1 = hist[i].gamma1;
    if(min.gamma2 > hist[i].gamma2) min.gamma2 = hist[i].gamma2; 
    if(max.gamma1 > hist[i].gamma1) max.gamma1 = hist[i].gamma1; 
    if(max.gamma2 > hist[i].gamma2) max.gamma2 = hist[i].gamma2; 
  }

  min.gamma1 = floor(min.gamma1/LBIN)*LBIN;
  min.gamma2 = floor(min.gamma2/LBIN)*LBIN;
  max.gamma1 = floor(max.gamma1/LBIN)*LBIN + LBIN;
  max.gamma2 = floor(max.gamma2/LBIN)*LBIN + LBIN;

  NBIN.gamma1 = (max.gamma1 - min.gamma1)/LBIN;
  NBIN.gamma2 = (max.gamma2 - min.gamma2)/LBIN;

  bin.gamma1 = (double*) calloc(NBIN, sizeof(double));
  if(bin.gamma1 == NULL) exit(-1);
  bin.gamma2 = (double*) calloc(NBIN, sizeof(double));
  if(bin.gamma2 == NULL) exit(-1);

  for(i=0; i<MAX; i++){
    index = (hist[i].gamma1 + min.gamma1)/LBIN;
    bin.gamma1[index]++;
    index = (hist[i].gamma2 + min.gamma2)/LBIN;
    bin.gamma2[index]++;
  }

  FILE *Hgamma1, *Hgamma2;
  Hgamma1 = fopen("gamma1.dat", "w");
  if(Hgamma1 == NULL){ error(100); }
  Hgamma2 = fopen("gamma2.dat", "w");
  if(Hgamma2 == NULL){ error(100); }

  for(i=0; i<BIN; i++){

    fprintf(Hgamma1,"%lf %.14lf\n", min.gamma1 + ( 0.5 + i ) * LBIN, bin.gamma1[i]/(double)MAX*LBIN);
    fprintf(Hgamma2,"%lf %.14lf\n", min.gamma2 + ( 0.5 + i ) * LBIN, bin.gamma2[i]/(double)MAX*LBIN);
  }

  free(hist);
  fclose(Hgamma1);
  fclose(Hgamma2);




}


void error(int errore){
  if(errore == 1){
    fprintf(stderr, "ERR(1), mancanza direttive da linea di comando\n"
            "inserire <Num. Estrazioni> <BIN istogramma>\n");
  }
  else if(errore == 100){
    fprintf(stderr,"ERROR 100: apertura file non avvenuta\n");
  }
  else if(errore == 64){
    fprintf(stderr,"ERROR: il sistema non ammette numeri a 64bit\n");
  }
  else if(errore == 101){
    fprintf(stderr,"ERROR 101, memoria non correttamente inizializzata\n");
  }
  else if(errore == 99){
    fprintf(stderr,"ERROR 99, non è stato eseguito nessun algoritmo causa bad coding\n");
  }
  exit(-1);
}