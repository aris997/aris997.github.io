#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define SHIFT 1000
#define MAX 10000
#define BIN 10

typedef unsigned long long int ullint;
typedef long int lint;

void error(int);
int statistica();

int main(){

  #ifdef CRONO
    clock_t begin = clock();
  #endif

  srand(time(NULL));
  srand48(time(NULL));

  if(sizeof(ullint) < 8){ error(64); }

  printf( "#\n# \tGenereratori di numeri casuali\n#\n"
          "# In questo codice sono implementati:\n"
          "# (0) GCL Minimal Standard, (1) RANDU, (2) L'ecuyer [64bit], (3) rand(),\n"
          "# (4) drand48(), (5) Shift-Register [ALL(?).]\n#\n"
          "# Si applicano test statistici: media, varianza, CHI quadro\n"
          "# Box-Muller(numeri casuali distribuiti normalmente\n"
          "# Sceglire quale generatore analizzare: ");

  int flag;
  do{
    scanf("%d", &flag);
    if (flag < 0 || flag > 5) printf( "\n# (0) GCL, (1) RANDU, "
                                      "(2) rand(), (3) drand48(), (4) Shift-Register\n"
          "# (5) L'ecuyer [64bit] [Per analizzarli tutti forse aggiorniamo.]\n"
          "Scegli meglio: ");
  }while(flag < 0 || flag > 5);

  ullint i;

  double ran;
  double sum=0, sum2=0;
  double INV_RANDMAX = 1./RAND_MAX;


  lint *hist;
  hist = (lint *)calloc(BIN, sizeof(lint));
  if(hist == NULL){error(101);}
  int bin;
  double x, y, z;

/*********************************************************************************/
  
  if(flag <= 2){

    double invM;

    FILE *output1;
    ullint m, a, seed;

    if(flag == 0){  //GCL - MINIMAL STANDARD
      
      printf("# GCL - Minimal Standard\n");
      m = (1ULL << 31) - 1ULL;
      a = 16807;
      seed = 756431;

      invM = 1./(double)m;

      output1 = fopen("minimalstd.dat", "w");
      if (output1 == NULL){error(100);}
    }

    if(flag == 1){  //GCL - The infamous RANDU

      printf("# GCL - RANDU\n");
      m = (1ULL << 31);
      a = 65539;
      seed = time(NULL);
      invM = 1./(double)m;


      output1 = fopen("randu.dat", "w");
      if (output1 == NULL){error(100);}
    }

    if(flag == 2){

      printf("#\n# L'Ecuyer type:1 64bit\n#\n");

      m = ULLONG_MAX;//( 1 << 64 ) - 1;
      a = 1181783497276652981ULL;
      seed = time(NULL);
      invM = 1./(double)m;

      output1 = fopen("ecuyer.dat", "w");
      if (output1 == NULL){error(100);}
    }

    ullint r = seed % m; //inizializzazione I_0


    for(i=0; i<MAX; i++){
      
      r = ( a * r ) % m;    //il GCL
      ran = (double)r * invM;
      
      sum += ran;       //statistiche
      sum2 += ran*ran;

      bin = (int)(ran*BIN);
      hist[bin]++;

      if(i%3 == 0) x = ran;
      else if(i%3 == 1) y = ran;
      else if(i%3 == 2){
        z = ran;
        fprintf(output1, "%lf %lf %lf\n", x, y, z);
      }
    }

    fclose(output1);
  }

/*********************************************************************************/

  else if(flag <= 4){ // rand() e drand48()


    if(flag == 2){

      FILE *output2;
      output2 = fopen("rand.dat", "w");
      if(output2 == NULL){error(100);}

      for(i=0; i<MAX; i++){
        ran = rand()*INV_RANDMAX;
          
        sum += ran;       //statistiche
        sum2 += ran*ran;

        bin = (int)(ran*BIN);
        hist[bin]++;

        if(i%3 == 0) x = ran;
        else if(i%3 == 1) y = ran;
        else if(i%3 == 2){
          z = ran;
          fprintf(output2, "%lf %lf %lf\n", x, y, z);
        }
      }
      fclose(output2);
    }

    else if(flag == 3){

      FILE *output3;
      output3 = fopen("lrand48.dat", "w");
      if(output3 == NULL){error(100);}

      for(i=0; i<MAX; i++){

        ran = lrand48()*INV_RANDMAX;

        sum += ran;       //statistiche
        sum2 += ran*ran;

        bin = (int)(ran*BIN);
        hist[bin]++;

        if(i%3 == 0) x = ran;
        else if(i%3 == 1) y = ran;
        else if(i%3 == 2){
          z = ran;
          fprintf(output3, "%lf %lf %lf\n", x, y, z);
        }
      }
      fclose(output3);
    }
  }
/*********************************************************************************/
  else if(flag == 5){ //  SHIFT REGISTER

    int addB = 31;
    int addC = 0;

    int BITS = 8 * sizeof(ullint);

    int k;

    int THEBIT;

    FILE *output5;
    output5 = fopen("shiftregister.dat", "w");
    if(output5 == NULL){error(100);}

    ullint *SR, r;
    SR = (ullint *) calloc(SHIFT, sizeof(ullint));
    if(SR == NULL) {error(101);}

    for(i=0; i<1000; i++) lrand48();

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

      sum += ran;
      sum2 += ran*ran;

      bin = (int)(ran*BIN);
      hist[bin]++;

      if(i%3 == 0) x = ran;
      else if(i%3 == 1) y = ran;
      else if(i%3 == 2){
        z = ran;
        fprintf(output5, "%lf %lf %lf\n", x, y, z);
      }
    }
    free(SR);
    fclose(output5);
  }



  FILE *outputH;
  outputH = fopen("hist.dat", "w");
  if(outputH == NULL){error(100);}

  ullint phi;

  //DATI PER ISTOGRAMMA
  for(i=0; i<BIN; i++){
    fprintf(outputH,"%lld %ld\n", i, hist[i]);
    phi += hist[i]*hist[i];
  }
  free(hist);
  fclose(outputH);

  //METODI STATISTICI
  double media = sum/(double)MAX;
  double varianza = (sum2 + (double)MAX * media * media - 2. * media * sum)/(double)(MAX - 1);
  double chi = (double)phi * (double)BIN/(double)MAX - (double)MAX;


  printf("media: %lf varianza: %lf chi su %u GDL: %lf\n", media, varianza, BIN-1, chi);




  #ifdef CRONO
    clock_t end = clock();
    printf("Execution time:%lf\n", (double)(end-begin)/CLOCKS_PER_SEC);
  #endif    
  exit(0);
}


int statistica(double ran, ){





  return bin;
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