#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define L 20
#define TMAX 2000
#define NSTORIES 10
#define RHO 0.20
#define WIDTH 10
#define MEASURES 1000

//RODARI-RIVA   30 NOVEMBRE 2017    v1.0.0    lfc409

/**************GAS RETICOLARE***************/
//
//
//
//

typedef long long int position;
typedef unsigned long long int steps;

typedef struct coord{
  long int x;
  long int y;
} coord;

void errori(int);
void init(int [][L]);
void fprinter(int [][L]);
void initdouble(double [][MEASURES]);
void initNeighbor(long int*,long int*);
//void Sites2Particels(coord*,int [][L],int);
void seedingParticels(int [][L],coord*,coord*,coord*,int);
void particelswalk(int [][L],coord*,coord*,long int*,long int*,int);
void measurements(coord*,coord*,double*,double [][MEASURES],int,int,int);


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
  int N = L2*rho;

  #ifdef VERBOSE_MODE
  printf("Particels: %d\n", N);
  #endif
  
  double Drho[MEASURES];

  int site[L][L];

  
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
  meanDR2 = (double *) calloc(N, sizeof(double));
  if (meanDR2 == NULL) errori(100);

  long int Dnarrow[L], Snarrow[L];
  initNeighbor(Dnarrow, Snarrow);


  for(i=0; i<NSTORIES; i++){
  
    init(site);
    seedingParticels(site, particels, initsite, truesite, N);

    #ifdef VERBOSE_MODE
      printf("Initialization termined, story:%llu\n", i+1);
    #endif

    for(T=0; T<TMAX; T++){
      particelswalk(site, particels, truesite, Dnarrow, Snarrow, N);
      if ((T % WIDTH) == 0) measurements(truesite, initsite, meanDR2, MEASURES, i, N);
    }
    
    #ifdef VISUAL_MODE
      fprinter(site);
    #endif

  }





      /******Analisi Dati******/


  #ifdef RAWDATA
    printf("DEFINED RAWDATA: STARTING F_PRINTING THE RAW ERRORS FILE\n");
    
    FILE *output1;
    output1 = fopen("errDR2.dat", "w");
    fprintf(output1, "#RAW DATA --GAS RETICOLARE--\n#Story\tMesure\terrDR2\n");
    
    for(i=0; i<NSTORIES; i++){
      for(T=0; T<MEASURES; T++){
        fprintf(output1, "%llu\t%llu\t%lf\n", i+1, T+1, errDR2[i][T]);
      }
    }
    
    #ifdef DEBUG_MODE
      printf("writing on outputX: OK\n");
    #endif
    
    fclose(output1);
    
    #ifdef DEBUG_MODE
      printf("outputX - CLOSED\n");
    #endif

  #endif

  free(initsite);
  free(truesite);
  free(particels);
  free(meanDR2);
  #ifdef VERBOSE_MODE 
    printf("initsite\n");
    printf("truesite\n");
    printf("particels\n");
    printf("meanDR2\n");
    printf("close output\n");
  #endif
  
  #ifdef CRONO_MODE
    clock_t end = clock();
    printf("Execution time: %lf\n", (double)(end-begin)/(double)CLOCKS_PER_SEC);
  #endif
  
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


void initNeighbor(long int Dnarrow[], long int Snarrow[]){

  int i;
  for(i=0; i<L; i++){
    Dnarrow[i] = i + 1;
    Snarrow[i] = i - 1;
  }

  Dnarrow[L-1] = 0;
  Snarrow[0] = L - 1; 
}

void seedingParticels(int site[L][L], coord particels[], coord initstate[], coord truesite[], int N){

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

void particelswalk(int site[L][L], coord particels[], coord truesite[], long int Dnarrow[], long int Snarrow[], int N){
  
  int p, r;
  int x, y;

  p = lrand48()%N + 1;
  r = lrand48()%4;


  x = particels[p].x;
  y = particels[p].y;
  
  #ifdef VERBOSE_MODE
  printf("random particel choosed: %d\trandom direction choosed: %d", p, r);
  printf("\t:%d\tx:%d\ty:%d", site[x][y], x, y);
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
    printf(" 0");
  }
  printf("\n");
  #endif
}







void measurements(coord truesite[], coord initsite[], double meanDR2[], double errDR2[NSTORIES][MEASURES], int t, int storia, int N){
  int p;
  for(p=1; p<=N; p++) {
    errDR2[storia][t] += pow(( truesite[p].x - initsite[p].x ), 2) + pow(( truesite[p].y - initsite[p].y ), 2);
  }
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