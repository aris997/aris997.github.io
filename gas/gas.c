#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define L 80
#define RHO 0.25
#define MEASURES (TMAX/WIDTH)
#define TMAX 2000
#define WIDTH 10
#define NSTORIES 10

//RODARI-RIVA   30 NOVEMBRE 2017    v1.0.0    lfc409

/**************GAS RETICOLARE***************/
//
//
//
//

typedef long long int pos;
typedef unsigned long long int steps;

typedef struct coord{
  pos x;
  pos y;
} coord;

void errori(int);
void init(pos [][L]);
void fprinter(pos [][L]);
void check_diocane(pos [][L]);
void initNeighbor(long int*,long int*);
void measurements(coord*,coord*,double*,int,int);
void seedingParticels(pos [][L],coord*,coord*,coord*,int);
void particelswalk(pos [][L],coord*,coord*,long int*,long int*,int);


int main () {
  #ifdef CRONO_MODE
  clock_t begin = clock();
  #endif
  srand48(time(NULL));

  //static int MEASURES = TMAX/WIDTH;
  //static int L = 20;

  double rho = RHO;
  steps T, i;
  int L2 = L*L;
  pos N = L2*rho;

  #ifdef VERBOSE_MODE
  printf("Particels: %lld\n", N);
  #endif
  
  //double Drho[MEASURES];

  pos site[L][L];

  
  coord *initsite;
  initsite = (coord *) calloc(N+1, sizeof(coord));
  if (initsite == NULL) errori(100);
  coord *truesite;
  truesite = (coord *) calloc(N+1, sizeof(coord));
  if (truesite == NULL) errori(100);
  coord *particels;
  particels = (coord *) calloc(N+1, sizeof(coord));
  if (particels == NULL) errori(100);
  double *meanDR2;
  meanDR2 = (double *) calloc(MEASURES, sizeof(double));
  if (meanDR2 == NULL) errori(100);

  long int Dnarrow[L], Snarrow[L];
  initNeighbor(Dnarrow, Snarrow);


  for(i=0; i<NSTORIES; i++){
  
    init(site);
    seedingParticels(site, particels, initsite, truesite, N);
    check_diocane(site);

    #ifdef VERBOSE_MODE
      printf("Initialization termined, story:%llu\n", i+1);
    #endif

    for(T=0; T<TMAX; T++){
      particelswalk(site, particels, truesite, Dnarrow, Snarrow, N);
      if ((T % WIDTH) == 0) measurements(truesite, initsite, meanDR2, T/WIDTH, N);
    }
    
    #ifdef VISUAL_MODE
      fprinter(site);
    #endif

  }





      /******Analisi Dati******/


  free(initsite);
  free(truesite);
  free(particels);
  free(meanDR2);

  
  #ifdef CRONO_MODE
    clock_t end = clock();
    printf("Execution time: %lf\n", (double)(end-begin)/(double)CLOCKS_PER_SEC);
  #endif
  
  exit(0);
}

void init(pos site[L][L]){
  
  int x, y;
  
  for(x=0; x<L; x++){
    for(y=0; y<L; y++){
      site[x][y] = 0;
    }
  }
}



void initNeighbor(long int Dnarrow[], long int Snarrow[]){

  int i;
  for(i=0; i<L; i++){
    Dnarrow[i] = i + 1;
    Snarrow[i] = i - 1;
  }

  Dnarrow[L-1] = 0;
  Snarrow[0] = L - 1; 
}

void seedingParticels(pos site[L][L], coord particels[], coord initstate[], coord truesite[], int N){

  int p, x, y;

  for (p=1; p<=N; p++){
    do { //SE NEL SITO VI È GIÀ UNA PARTICELLA, RIGENERA DELLE coord RANDOM
      x = lrand48()%L;
      y = lrand48()%L;
    } while(site[x][y] > 0);
    site[x][y] = p;
    particels[p].x = x;
    particels[p].y = y;
    initstate[p].x = x;
    initstate[p].y = y;
    truesite[p].x = x;
    truesite[p].y = y;
  }
}

void check_diocane(pos site[L][L]){
  int x, y;
  for(x=0; x<L; x++){
    for(y=0; y<L; y++){
      if (site[x][y] > RHO*L*L) {
        printf("PORCACCIO IL DIO MALEDETTO INFAME BOIA\n");
        exit(-1);
      }
    }
  }
}



// void Sites2Particels(coord state[], int site[L][L], int N){
  
//   int x, y;
//   int p=0;
  
//   for(x = 0; x < L; x++){
//     for(y = 0; y < L; y++){
//       if(site[x][y] > 0){
//         state[p].x = x;
//         state[p].y = y;
//         p++;
//       }
//       #ifdef DEBUG_MODE
//         if (p > N) errori(99);
//       #endif
//     }
//   }
// }


////////////////////////////////////////////////////////////////////////////////
//***************************PARTICEL WALK FUNCTION***************************//
////////////////////////////////////////////////////////////////////////////////

void particelswalk(pos site[L][L], coord particels[], coord truesite[], long int Dnarrow[], long int Snarrow[], int N){
  
  int p, r;
  int x, y;

  p = lrand48()%N + 1;
  r = lrand48()%4;


  x = particels[p].x;
  y = particels[p].y;
  
  #ifdef VERBOSE_MODE
  printf("p:%d\tdir:%d", p, r);
  printf("\tsite:%lld\tx:%d\ty:%d", site[x][y], x, y);
  #endif

  //la direzione del movimento è in senso orario r: 0,1,2,3 -> Nord, Est, Sud, Ovest
  if (r == 0 && site[x][Dnarrow[y]] == 0){
    site[x][Dnarrow[y]] = site[x][y];
    site[x][y] = 0;
    particels[p].y = Dnarrow[y];
    truesite[p].y++;
  }

  else if (r == 1 && site[Dnarrow[x]][y] == 0){
    site[Dnarrow[x]][y] = site[x][y];
    site[x][y] = 0;
    particels[p].x = Dnarrow[x];
    truesite[p].x++;
  }

  else if (r == 2 && site[x][Snarrow[y]] == 0){
    site[x][Snarrow[y]] = site[x][y];
    site[x][y] = 0;
    particels[p].y = Snarrow[y];
    truesite[p].y--;
  }

  else if (r == 3 && site[Snarrow[x]][y] == 0){
    site[Snarrow[x]][y] = site[x][y];
    site[x][y] = 0;
    particels[p].x = Snarrow[x];
    truesite[p].x--;
  }
  #ifdef VERBOSE_MODE
  else {
    printf("\t0");
  }
  printf("\n");
  #endif
}







void measurements(coord truesite[], coord initsite[], double meanDR2[], int t, int N){
  int p=0;
  
  for(p=1; p<=N; p++) {  
    meanDR2[t+1] += pow(( truesite[p].x - initsite[p].x ), 2) + pow(( truesite[p].y - initsite[p].y ), 2);
  }
}








void fprinter(pos site[L][L]){

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