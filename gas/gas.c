#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define L 20
#define RHO 0.2
#define MEASURES (TMAX/WIDTH)
#define TMAX 2000
#define WIDTH 10
#define NSTORIES 1000

//RODARI-RIVA   30 NOVEMBRE 2017    v1.0.0    labfc409

/******************************************GAS RETICOLARE******************************************/
// -    lrand48 genera posizioni casuali nell'intervallo (0,L), si tiene conto della particella
//    per tutto il programma numerandole una ad una.
// -    Il sistema da spesso errore, a causa della facilità con cui si superano le dimensioni
//    dell'architettura del computer. È quindi consigliato in tali casi compilare il codice
//    inserendo il define "-D DEBUG_MODE" o "-D VERBOSE_MODE", la prima è principalmente di debug
//    mentre la seconda è stata utile per scovare delle discordanze nel codice.
//      Altri define sono "-D VISUAL_MODE", stampa un file di prova per poter osservare le
//    posizioni delle particelle; "-D CRONO_MODE" restituisce un eseguibile che misura il suo
//    tempo di esecuzione.
// -    Nel codice più volte sono usciti errori particolari, essi sono stati raggruppati in una
//    funzione che stampa su schermo l'oggetto dell'errore ed esce dal codice senza riportare
//    segmentation fault o altri casi problematici. Gli errori implementati fin'ora sono 99,
//    100, 101. Ciascuno ha una causa specifica.

typedef long long int pos;
typedef unsigned long long int steps;

typedef struct coord{
  pos x;
  pos y;
} coord;

void errori(int);
void init(pos [][L]);
void check(pos [][L]);
void somme(pos [][L]);
void fprinter(pos [][L]);
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
    check(site);

    #ifdef VERBOSE_MODE
      printf("Initialization termined, story:%llu\n", i+1);
    #endif

    for(T=0; T<TMAX; T++){
      particelswalk(site, particels, truesite, Dnarrow, Snarrow, N);
      if((T % WIDTH) == 0) measurements(truesite, initsite, meanDR2, T/WIDTH, N);
      somme(site);
    }
    
    #ifdef VISUAL_MODE
      fprinter(site);
    #endif
  }

      /******Analisi Dati******/

  FILE *output1;
  output1 = fopen("drho.dat", "w");
  fprintf(output1, "#deltaRquadro(t)\n");
  for(i=1; i<MEASURES; i++){
    meanDR2[i]/=NSTORIES;
    fprintf(output1, "%lf\n", 1./meanDR2[i]);
    printf("%lf\n", 1./meanDR2[i]);
  }




  free(initsite);
  free(truesite);
  free(particels);
  free(meanDR2);
  fclose(output1);

  
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

void check(pos site[L][L]){
  int x, y;
  for(x=0; x<L; x++) for(y=0; y<L; y++) if(site[x][y] > RHO*L*L) errori(101);
}

void somme(pos site[L][L]){
  int x, y, p=0;
  for(x=0; x<L; x++) for(y=0; y<L; y++) if(site[x][y] > 0) p++;
  if(p>(RHO*L*L)) {
    printf("particels:%d\n", p);
    errori(99);
  }
}
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
  
  #ifdef LOG_MODE
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
  #ifdef LOG_MODE
  else {
    printf("\t0");
  }
  printf("\n");
  #endif
}

void measurements(coord truesite[], coord initsite[], double meanDR2[], int t, int N){
  int p=0;
  double R2=0.;
  for(p=1; p<=N; p++){
    R2 += pow((truesite[p].x-initsite[p].x),2) + pow((truesite[p].y-initsite[p].y),2);
  }
  R2/=(double)N;
  meanDR2[t+1] += R2;
}

void fprinter(pos site[L][L]){

  int x, y;
  FILE *op;
  op = fopen("data.dat", "w");
  fprintf(op, "#Coordinates of seeded points (index is the particle name)\n");
  for(x=0; x<L; x++){
    for(y=0; y<L; y++){
      if (site[x][y] > 0) fprintf(op, "%d %d\n", x, y);
    }
  }
  fclose(op);
}

void errori(int n){
  if (n == 100){ //calloc wrong
    printf("ERROR 100\nCALLOC PROBLEMS\n");
  }
  else if(n == 99){
    printf("ERROR 99\ntoo many particels but some of them doesn't exist!!!\n");
  }
  else if(n == 101){
    printf("REVIEW DEFINES, U MADE A MISTAKE\n");
  }
  exit(-1);
}