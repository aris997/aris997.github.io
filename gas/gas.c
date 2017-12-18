#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define MEASURES 10
#define D 2

//RODARI-RIVA   30 NOVEMBRE 2017    v2.0.0    labfc409

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

typedef unsigned long long int ullint;
typedef long long int llint;
typedef long int lint;

typedef struct coord{
  llint x;
  llint y;
} coord;

void init(int**,int);
void initNeighbor(long int*,long int*,int);

void calcheck_d(double*);
void calcheck_c(coord*);
void error(int);




int main(int argc, char *argv[]){

  #ifdef CRONO_MODE
    clock_t begin = clock();
  #endif

  srand(time(NULL));



  int L, label, N, T, r, i, j, k;
  lint Tmax, Nstories, x, y;
  double rho, prob;

  if(argc == 1){ //conviene creare una funzione qui, ho il cancro a mettere tutti sti if nel main
    error(1);
    L = 20;
    rho = 0.2;
    Tmax = 1000;
    Nstories = 100;
    fprintf(stdout,"L:%d rho:%lf Tmax:%ld Nstories:%ld\n", L, rho, Tmax, Nstories);
  }

  else if(argc == 5){
    L = atol(argv[1]);
    rho = atof(argv[2]);
    Tmax = atol(argv[3]);
    Nstories = atol(argv[4]);
    fprintf(stdout,"L:%d rho:%lf Tmax:%ld Nstories:%ld\n", L, rho, Tmax, Nstories);
  }
  
  else error(1);



  int **site;
  site = (int**) calloc(L, sizeof(int*));
  if(site == NULL) error(2);
  for(i=0; i<L; i++){
    site[i] = (int *) calloc(L, sizeof(int));
    if (site[i] == NULL) error(2);
  }

  double *RWSUM, *RW;
  RWSUM = (double *) calloc(Tmax, sizeof(double));
  calcheck_d(RWSUM);


  lint plus[L], less[L];
  initNeighbor(plus, less, L);

  coord *initpos, *truepos, *condpos;

  for(i=0; i<Nstories; i++){
  
    init(site, L); //reticolo azzerato
    initpos = (coord *) calloc(L*L, sizeof(coord));
    calcheck_c(initpos); //controllo allocazione
    truepos = (coord *) calloc(L*L, sizeof(coord));
    calcheck_c(truepos); //controllo allocazione
    condpos = (coord *) calloc(L*L, sizeof(coord));
    calcheck_c(condpos); //controllo allocazione
    RW = (double *) calloc(Tmax, sizeof(double));
    calcheck_d(RW); //controllo allocazione

    N = 0; //particelle inserite a ogni step


    for(j=0; j<L; j++){
      for(k=0; k<L; k++){
        prob = (double)rand()/RAND_MAX;
        if(prob < rho){
          
          site[j][k] = 1;

          initpos[N].x = j;
          initpos[N].y = k;
          truepos[N].x = j;
          truepos[N].y = k;
          condpos[N].x = j;
          condpos[N].y = k;

          N++;
        }
      }
    }

    //fprintf(stderr,"%d\n",N);


    for(T=0; T<Tmax; T++){
        
      for(k=0; k<N; k++){

        label = rand()%N;
        x = condpos[label].x;
        y = condpos[label].y;

        r = rand()%4; //0 - Nord, 1 - Est, 2 - Sud, 3 - Ovest

        if (r == 0 && site[x][plus[y]] == 0){
          site[x][plus[y]] = 1;
          site[x][y] = 0;
          condpos[label].y = plus[y];
          truepos[label].y++;
        }

        else if (r == 1 && site[plus[x]][y] == 0){
          site[plus[x]][y] = 1;
          site[x][y] = 0;
          condpos[label].x = plus[x];
          truepos[label].x++;
        }

        else if (r == 2 && site[x][less[y]] == 0){
          site[x][less[y]] = 1;
          site[x][y] = 0;
          condpos[label].y = less[y];
          truepos[label].y--;
        }

        else if (r == 3 && site[less[x]][y] == 0){
          site[less[x]][y] = 1;
          site[x][y] = 0;
          condpos[label].x = less[x];
          truepos[label].x--;
        }
      }

      if(T%MEASURES == 0){
        for (k=0; k<N; k++){
          RW[T] += pow((truepos[k].x-initpos[k].x),2) + pow((truepos[k].y-initpos[k].y),2);
        }
        RW[T] /= N;
      }
    }


    for (k=0; k<Tmax; k++){
        RWSUM[k] += RW[k];
    }



    free(initpos);
    free(truepos);
    free(condpos);
    free(RW);

    fprintf(stderr,"fine storia%d\n", i);

  }

  FILE *output1;
  char filename[50];
  sprintf(filename, "drho%02d_%02d.dat", L, (int)(rho*10));
  output1 = fopen(filename, "w");
  fprintf(output1, "#deltaRquadro(t)\n");
  for(i=0; i<Tmax; i+=MEASURES){
    fprintf(output1, "%d %.14lf\n", i+1, ((double)RWSUM[i]/Nstories)/(2.*D*(i+1.)));
  }

  fclose(output1);


  #ifdef CRONO_MODE
    clock_t end = clock();
    fprintf(stdout,"# Execution time:%lf\n", (double)(end-begin)/(double)CLOCKS_PER_SEC);
  #endif

    fprintf(stderr,"arrivo qui\n");

  exit(0);
}




void init(int **site, int L){
  
  int x, y;
  
  for(x=0; x<L; x++){
    for(y=0; y<L; y++){
      site[x][y] = 0;
    }
  }
}

void initNeighbor(lint plus[], lint less[], int L){

  int i;
  for(i=0; i<L; i++){
    plus[i] = i + 1;
    less[i] = i - 1;
  }

  plus[L-1] = 0;
  less[0] = L - 1; 
}

void calcheck_d(double array[]){
  if (array == NULL){
    printf("ERROR calloc of double size not set\ngo back home, it's cold outside\n");
    exit(-1);
  }
  return;
}

void calcheck_c(coord array[]){
  if (array == NULL){
    printf("ERROR calloc of coord size not set\ngo back home, it's cold outside\n");
    exit(-1);
  }
  return;
}


void error(int n){
  if(n == 1){
    fprintf(stderr,"Insert from line <Lenght> <rho> <Tmax> <Nstories>\n");
  }
  else exit(-1);
}