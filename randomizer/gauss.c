#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define PI 3.1415926535897932384626433832795

#define MEDIA 1
#define SHIFT 1000
#define MAX 100000
#define BIN 15

typedef unsigned long long int ullint;
typedef long int lint;

void error(int);
int statistica(double,double,double);

int main(){

  #ifdef CRONO
    clock_t begin = clock();
  #endif

  srand(time(NULL));
  srand48(time(NULL));

  if(sizeof(ullint) < 8){ error(64); }

  printf( "#\n# \tGENERERATORI DI NUMERI PSEUDOCASUALI\n# \t\tdistribuzione Gaussiana\n#\n"

          "# Si applicano test statistici: media, varianza, CHI quadro\n"
          "# e media dei CHI quadro.\n"
          "# Vengono stampati su file i dati per\n"
          "# plottare Istogrammi su gnuplot e dati per analizzare i\n"
          "# generatori con coordinate:\n#\n#\t\t"
          " x = r_n   y = r_(n+1)   z = r_(n+2).\n#\n"
          "# Sceglire quale generatore analizzare,\n"
          "# in questo codice sono implementati:\n");

  int flag, choice, stories=1;

  do{
    printf("# (0) GCL MinSTD, (1) RANDU, (2) L'ecuyer [64bit], (3) rand(),\n"
           "# (4) drand48(), (5) Shift-Register,\n"
           "# Scegliere (6) per eseguirli tutti\n# ");
    scanf("%d", &choice);
  }while(choice < 0 || choice > 6);

  if(choice == 6) stories = 6;



  register ullint i;

  for(flag=0; flag<stories; flag++){ /////////CICLO PER TUTTI GLI ALGORITMI/////////////////////////

    if(choice < 6)flag = choice;

    double ran = 0;
    double ran1 = 0;
    double ran2 = 0;
    double INV_RANDMAX = 1./RAND_MAX;



    lint *hist;
    hist = (lint *)calloc(BIN*10, sizeof(lint));
    if(hist == NULL){error(101);}
    
    int bin;

    /********************************************************************************************/
    
    if(flag <= 2){

      double invM;

      ullint m, a, seed;

      if(flag == 0){  //GCL - MINIMAL STANDARD
        
        printf("#\n# GCL - Minimal Standard - FIXED SEED\n");
        m = (1ULL << 31) - 1ULL;
        a = 16807;
        seed = 756431;
      }

      if(flag == 1){  //GCL - The infamous RANDU

        printf("#\n# GCL - RANDU\n");
        m = (1ULL << 31);
        a = 65539;
        seed = time(NULL);
      }

      if(flag == 2){ //L'ecuyer type:1 -> 64bit

        printf("#\n# L'Ecuyer type:1 64bit\n");
        m = ULLONG_MAX;//( 1 << 64 ) - 1;
        a = 1181783497276652981ULL;
        seed = time(NULL);
      }

      invM = 1./(double)m;
      ullint r = seed % m; //inizializzazione I_0

      for(i=0; i<MAX; i++){
        
        r = ( a * r ) % m;    //il GCL
        ran = (double)r * invM;

        r = ( a * r ) % m;    //il GCL
        ran1 = (double)r * invM;

        r = ( a * r ) % m;    //il GCL
        ran2 = (double)r * invM;
        
        bin = statistica(ran, ran1, ran2);
        hist[bin]++;
      }
    }

    /********************************************************************************************/

    else if(flag <= 4){ // rand() e drand48()

      if(flag == 3){

        printf("#\n# rand()\n");

        for(i=0; i<MAX; i++){

          ran = rand()*INV_RANDMAX;
          ran1 = rand()*INV_RANDMAX;
          ran2 = rand()*INV_RANDMAX;

          bin = statistica(ran, ran1, ran2);
          hist[bin]++;
        }
      }

      else if(flag == 4){

        printf("#\n# lrand48()\n");

        for(i=0; i<MAX; i++){

          ran = lrand48()*INV_RANDMAX;
          ran1 = lrand48()*INV_RANDMAX;
          ran2 = lrand48()*INV_RANDMAX;

          bin = statistica(ran, ran1, ran2);
          hist[bin]++;
        }
      }
    }

    /********************************************************************************************/
    else if(flag == 5){ //  SHIFT REGISTER

      printf("#\n# Shift-Register [inizializzato con lrand48()]\n");
      int addB = 31;
      int addC = 0;

      int BITS = 8 * sizeof(ullint);

      int k;

      ullint *SR, r, THEBIT;

      SR = (ullint *) calloc(SHIFT, sizeof(ullint));
      if(SR == NULL) {error(101);}

      for(i=0; i<1000; i++) lrand48(); //lrand48 gira 1000, inizializzandosi

      for(i=0; i<SHIFT; i++){
        
        for(k=0; k<BITS; k++){
          
          ran = (double)lrand48()*INV_RANDMAX;

          if(ran < 0.5) THEBIT = 1;
          else THEBIT = 0;

          SR[i] = SR[i] | (THEBIT << k);
        }
      }

      for(i=0; i<MAX; i++){

        r = SR[addB] ^ SR[addC];

        SR[addC] = r;
        addC++;
        addB++;

        if(addB == SHIFT) addB = 0;
        if(addC == SHIFT) addC = 0;
      
        ran = (double)r/(double)ULLONG_MAX;

        r = SR[addB] ^ SR[addC];

        SR[addC] = r;
        addC++;
        addB++;

        if(addB == SHIFT) addB = 0;
        if(addC == SHIFT) addC = 0;
      
        ran1 = (double)r/(double)ULLONG_MAX;

        r = SR[addB] ^ SR[addC];

        SR[addC] = r;
        addC++;
        addB++;

        if(addB == SHIFT) addB = 0;
        if(addC == SHIFT) addC = 0;
      
        ran2 = (double)r/(double)ULLONG_MAX;


        bin = statistica(ran, ran1, ran2);
        hist[bin]++;
      }
      free(SR);
    }

    else exit(-1);

    //********************************************************************************************
    //ISTOGRAMMA in outputH - 
   
    FILE *outputH;
    char filename[12];
  
    //vengono stampati istogrammi per ogni gen se si vogliono tutti
    sprintf(filename, "gauss%02d.dat", flag);
    outputH = fopen(filename, "w");
    if(outputH == NULL){ error(100); }

    ullint phi=0;


    for(i=0; i<10*BIN; i++){

      fprintf(outputH,"%lld %.14lf\n", i-(int)0.5*BIN, (double)hist[i]/MAX);
      phi += hist[i]*hist[i]; //Sfrutto questo ciclo per le somme quadrate necessarie al CHIquadro
    }

    free(hist);
    fclose(outputH);
    //********************************************************************************************



  } //////////////////////////////////FINE CICLO ALGORITMI//////////////////////////////////////////


  #ifdef CRONO
    clock_t end = clock();
    printf("# Execution time:%lf\n", (double)(end-begin)/CLOCKS_PER_SEC);
  #endif    


  exit(0);
}



int statistica(double ran, double ran1, double ran2){


  //BOX-MULLER

  double g, g1, g2, arg1, arg2;

  arg1 = sqrt( - 2. * log(ran1) );
  arg2 = 2. * PI * ran2;

  g1 = arg1 * cos(arg2);
  g2 = arg1 * sin(arg2);

  if(ran < 0.5) g = g1;
  else g = g2;

  return (int)( g*BIN + 5*BIN );
}

void error(int errore){
  if(errore == 100){
    printf("ERROR 100: apertura file non avvenuta\n");
  }
  else if(errore == 64){
    printf("ERROR: il sistema non ammette numeri a 64bit\n");
  }
  else if(errore == 101){
    printf("ERROR 101, memoria non correttamente inizializzata\n");
  }
  exit(-1);
}