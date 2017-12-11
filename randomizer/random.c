#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define PI 3.1415926535897932384626433832795

#define MEDIA 1
#define SHIFT 1000
#define MAX 1000
#define BIN 10

typedef unsigned long long int ullint;
typedef long int lint;

void error(int);
int statistica(double,double*,double*,ullint,FILE*);

int main(){

  #ifdef CRONO
    clock_t begin = clock();
  #endif

  srand(time(NULL));
  srand48(time(NULL));

  if(sizeof(ullint) < 8){ error(64); }

  printf( "#\n# \tGENERERATORI DI NUMERI PSEUDOCASUALI\n#\n"

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
    int S = 0; //Variabile di controllo scorrimento del ciclo delle medie
    double chiq = 0;

    do { //CICLO DO-WHILE per mediare su CHIquadro


      double ran = 0;
      double sum=0, sum2=0;
      double INV_RANDMAX = 1./RAND_MAX;



      lint *hist;
      hist = (lint *)calloc(BIN, sizeof(lint));
      if(hist == NULL){error(101);}
      int bin;

      /********************************************************************************************/
      
      if(flag <= 2){

        double invM;

        FILE *output1;
        ullint m, a, seed;

        if(flag == 0){  //GCL - MINIMAL STANDARD
          
          printf("#\n# GCL - Minimal Standard - FIXED SEED\n");
          m = (1ULL << 31) - 1ULL;
          a = 16807;
          seed = 756431;

          output1 = fopen("minimalstd.dat", "w");
          if (output1 == NULL){error(100);}
        }

        if(flag == 1){  //GCL - The infamous RANDU

          printf("#\n# GCL - RANDU\n");
          m = (1ULL << 31);
          a = 65539;
          seed = time(NULL);

          output1 = fopen("randu.dat", "w");
          if (output1 == NULL){error(100);}
        }

        if(flag == 2){ //L'ecuyer type:1 -> 64bit

          printf("#\n# L'Ecuyer type:1 64bit\n");
          m = ULLONG_MAX;//( 1 << 64 ) - 1;
          a = 1181783497276652981ULL;
          seed = time(NULL);

          output1 = fopen("ecuyer.dat", "w");
          if (output1 == NULL){error(100);}
        }

        invM = 1./(double)m;
        ullint r = seed % m; //inizializzazione I_0

        for(i=0; i<MAX; i++){
          
          r = ( a * r ) % m;    //il GCL
          ran = (double)r * invM;

          bin = statistica(ran, &sum, &sum2, i, output1);
          hist[bin]++;
        }
        fclose(output1);
      }

      /********************************************************************************************/

      else if(flag <= 4){ // rand() e drand48()

        if(flag == 3){

          printf("#\n# rand()\n");
          FILE *output3;
          output3 = fopen("rand.dat", "w");
          if(output3 == NULL){error(100);}

          for(i=0; i<MAX; i++){

            ran = rand()*INV_RANDMAX;
            bin = statistica(ran, &sum, &sum2, i, output3);
            hist[bin]++;
          }
          fclose(output3);
        }

        else if(flag == 4){

          printf("#\n# lrand48()\n");
          FILE *output4;
          output4 = fopen("lrand48.dat", "w");
          if(output4 == NULL){error(100);}

          for(i=0; i<MAX; i++){

            ran = lrand48()*INV_RANDMAX;
            bin = statistica(ran, &sum, &sum2, i, output4);
            hist[bin]++;
          }
          fclose(output4);
        }
      }

      /********************************************************************************************/
      else if(flag == 5){ //  SHIFT REGISTER

        printf("#\n# Shift-Register [inizializzato con lrand48()]\n");
        int addB = 31;
        int addC = 0;

        int BITS = 8 * sizeof(ullint);

        int k;

        FILE *output5;
        output5 = fopen("shiftregister.dat", "w");
        if(output5 == NULL){error(100);}

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

          bin = statistica(ran, &sum, &sum2, i, output5);
          hist[bin]++;
        }
        free(SR);
        fclose(output5);
      }

      else exit(-1);

      //********************************************************************************************
      //ISTOGRAMMA in outputH - 
     
      FILE *outputH;
      char filename[11];
    
      //vengono stampati istogrammi per ogni gen se si vogliono tutti
      sprintf(filename, "hist%02d.dat", flag);
      outputH = fopen(filename, "w");
      if(outputH == NULL){ error(100); }

      ullint phi=0;

      for(i=0; i<BIN; i++){
        fprintf(outputH,"%lld %ld\n", i, hist[i]);
        phi += hist[i]*hist[i]; //Sfrutto questo ciclo per le somme quadrate necessarie al CHIquadro
      }

      free(hist);
      fclose(outputH);
      //********************************************************************************************

      //Completamento della statistica
      double media = sum/(double)MAX;
      double varianza = (sum2 + (double)MAX * media * media - 2. * media * sum)/(double)(MAX - 1);
      double chi = (double)phi * (double)BIN/(double)MAX - (double)MAX;

      printf("# Storia[%d]\tmedia: %lf, varianza:"
             "%lf, chi su %u GdL: %lf\n", S+1, media, varianza, BIN-1, chi);
      

      chiq+=chi;

      S++; //Variabile di controllo, necessario per mediare ulteriormente sul CHIquadro
    } while(S<MEDIA);


    double chiM = chiq/MEDIA;

    printf("#\n# ALGORITMO[%d] media del chi quadro su %u GdL: \t\t\t%lf\n", flag, BIN-1, chiM);

  } //////////////////////////////////FINE CICLO ALGORITMI//////////////////////////////////////////


  #ifdef CRONO
    clock_t end = clock();
    printf("# Execution time:%lf\n", (double)(end-begin)/CLOCKS_PER_SEC);
  #endif    


  exit(0);
}



int statistica(double ran, double *sum, double *sum2, ullint i, FILE *output){

  (*sum) += ran;
  (*sum2) += ran*ran;

  fprintf(output, "%lf ", ran);
  if(i%3 == 2) fprintf(output, "\n");

  return (int)(ran*BIN);
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