#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define L 20
#define TMAX 10000
#define NSTORIES 10
#define RHO 0.20
#define GAPMEASURES 100
#define MEASURES 100

//RODARI-RIVA   30 NOVEMBRE 2017    v1.0.0    lfc409

/**************GAS RETICOLARE***************/
//
//
//
//

typedef long long int position;
typedef unsigned long long int steps;

typedef struct coordinate{
  long int x;
  long int y;
} coordinate;

void errori(int);
void init(int [][L]);
void fprinter(int [][L]);
void initdouble(double [][MEASURES]);
void seedingParticels(int [][L], int);
void initNeighbor(long int *, long int *);
void Sites2Particels(coordinate*, int [][L], int);
void particelswalk(int [][L], coordinate*, coordinate*, long int *, long int *, int);
void measurements(coordinate*, coordinate*, double *, double [][MEASURES], int, int, int);


int main () {
  clock_t begin = clock();
  srand48(time(NULL));

  //static int MEASURES = TMAX/GAPMEASURES;
  //static int L = 20;

  double rho = RHO;
  steps T, i;
  int L2 = L*L;
  int N = L2*rho;
  printf("Particels: %d\n", N);
  
  double Drho[MEASURES];

  int site[L][L];
  double errDR2[NSTORIES][MEASURES];

  initdouble(errDR2);
  
  coordinate *initsite;
  initsite = (coordinate *) calloc(N, sizeof(coordinate));
  if (initsite == NULL) errori(100);
  
  coordinate *truesite;
  truesite = (coordinate *) calloc(N, sizeof(coordinate));
  if (truesite == NULL) errori(100);

  coordinate *particels;
  particels = (coordinate *) calloc(N, sizeof(coordinate));
  if (particels == NULL) errori(100);

  double *meanDR2;
  meanDR2 = (double *) calloc(N, sizeof(double));
  if (meanDR2 == NULL) errori(100);


  long int plusneighbor[L], minusneighbor[L];
  initNeighbor(plusneighbor, minusneighbor);


  for(i=0; i<NSTORIES; i++){
  
    init(site);
    seedingParticels(site, N);
    Sites2Particels(initsite, site, N);
    Sites2Particels(truesite, site, N);
    Sites2Particels(particels, site, N);

    #ifdef VERBOSE_MODE
      printf("INITIALIZATION IS OVER\n");
    #endif

    for(T=0; T<TMAX; T++){
      particelswalk(site, particels, truesite, plusneighbor, minusneighbor, N);
      if ((T % GAPMEASURES) == 0) measurements(truesite, initsite, meanDR2, errDR2, T/GAPMEASURES, i, N);
    }
    
    #ifdef VERBOSE_MODE
      fprinter(site);
    #endif    
  }


      /******Analisi Dati******/

  double x2, meanx, VAR;
  double sd[MEASURES]={0};
  
  FILE *output;
  output = fopen("Drho.dat", "w");
  
  for(T=0; T<TMAX; T+=GAPMEASURES){  //calcolo degli errori
    x2=0.;
    meanx=0.;

    for(i=0; i<NSTORIES; i++) {
      x2 += errDR2[i][T]*errDR2[i][T];
      meanx += errDR2[i][T];
    }

    x2/=NSTORIES;
    meanx/=NSTORIES;
    VAR = x2 - meanx*meanx;
    sd[T] = pow(VAR, 0.5);
   
    meanDR2[T]/=MEASURES;
    Drho[T] = (1./(4.*(double)TMAX))*meanDR2[T];
  
    fprintf(output, "%.14lf\t%.14lf\n", Drho[T], sd[T]);
  }


  free(initsite);
  free(truesite);
  free(particels);
  fclose(output);

  clock_t end = clock();
  printf("Execution time: %lf\n", (double)(end-begin)/(double)CLOCKS_PER_SEC);
  exit(0);
}

void init(int site[L][L]){
  
  int x, y;
  
  for(x=0; x<L; x++){
    for(y=0; y<L; y++){
      site[x][y] = 0;
    }
  }
}

void initdouble(double errDR2[NSTORIES][MEASURES]){
  int n, m; 

  for(n=0; n<NSTORIES; n++){
    for(m=0; m<MEASURES; m++){
      errDR2[n][m]=0.;
    }
  }
}


void initNeighbor(long int plusneighbor[], long int minusneighbor[]){

  int i;
  for(i=0; i<L; i++){
    plusneighbor[i] = i + 1;
    minusneighbor[i] = i - 1;
  }

  plusneighbor[L-1] = 0;
  minusneighbor[0] = L - 1; 
}

void seedingParticels(int site[L][L], int N){

  int p, x, y;

  for (p=1; p<=N; p++){
    do { //SE NEL SITO VI È GIÀ UNA PARTICELLA, RIGENERA DELLE COORDINATE RANDOM
      x = lrand48()%L;
      y = lrand48()%L;
    } while(site[x][y] > 0);
    site[x][y] = p;
  }
}

void Sites2Particels(coordinate state[], int site[L][L], int N){
  
  int x, y;
  int p=0;
  
  for(x = 0; x < L; x++){
    for(y = 0; y < L; y++){
      if(site[x][y] > 0){
        state[p].x = x;
        state[p].y = y;
        p++;
      }
      #ifdef DEBUG_MODE
        if (p > N) errori(99);
      #endif
    }
  }
}


void particelswalk(int site[L][L], coordinate particels[], coordinate truesite[], long int plusneighbor[], long int minusneighbor[], int N){
  
  int p, r;
  int x, y;

  p = lrand48()%N;
  r = lrand48()%4;

  #ifdef VERBOSE_MODE
    printf("random particel choosed: %d\trandom direction choosed: %d\n", p, r);
  #endif

  x = particels[p].x;
  y = particels[p].y;
  
  //ho un numero per orientare la direzione del movimento in senso orario 0,1,2,3 -> Nord, Est, Sud, Ovest
  if (r == 0 && site[x][plusneighbor[y]] == 0){
    site[x][plusneighbor[y]] = site[x][y];
    site[x][y] = 0;
    particels[p].y = plusneighbor[y];
    truesite[p].y++;
  }

  else if (r == 1 && site[plusneighbor[x]][y] == 0){
    site[plusneighbor[x]][y] = site[x][y];
    site[x][y] = 0;
    particels[p].x = plusneighbor[x];
    truesite[p].x++;
  }

  else if (r == 2 && site[x][minusneighbor[y]] == 0){
    site[x][minusneighbor[y]] = site[x][y];
    site[x][y] = 0;
    particels[p].y = minusneighbor[y];
    truesite[p].y--;
  }

  else if (r == 3 && site[minusneighbor[x]][y] == 0){
    site[minusneighbor[x]][y] = site[x][y];
    site[x][y] = 0;
    particels[p].x = minusneighbor[x];
    truesite[p].x--;
  }
}

void measurements(coordinate truesite[], coordinate initsite[], double meanDR2[], double errDR2[NSTORIES][MEASURES], int t, int storia, int N){
  int p;
  for(p=0; p<N; p++) errDR2[storia][t] += pow(( truesite[p].x - initsite[p].x ), 2) + pow(( truesite[p].y - initsite[p].y ), 2);
  errDR2[storia][t]/=(double)N;
  meanDR2[t] += errDR2[storia][t];
}

void fprinter(int site[L][L]){

  int x, y;
  FILE *output;
  output = fopen("data.dat", "w");

  for(x=0; x<L; x++){
    for(y=0; y<L; y++){
      if (site[x][y] > 0) fprintf(output, "%d %d\n", x, y);
    }
  }
  fclose(output);
}

void errori(int n){
  if (n == 100){ //calloc wrong
    printf("ERROR 100\nCALLOC PROBLEMS\n");
  }
  else if(n == 99){
    printf("ERROR 99\ntoo many particels but some of them doesn't exist!!!\n");
  }
  exit(-1);
}