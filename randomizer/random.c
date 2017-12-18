#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define PI 3.1415926535897932384626433832795 //

#define SHIFT 1000

typedef unsigned long long int ullint;
typedef long int lint;

void error(int);

int main(int argc, char **argv){

  srand(time(NULL));
  srand48(time(NULL));

  if(argc != 3) error(1);
  int MAX = atoi(argv[1]);
  int BIN = atoi(argv[2]);

  clock_t begin, end;

  if(sizeof(ullint) < 8){ error(64); }

  printf( "#\n# \tGENERERATORI DI NUMERI PSEUDOCASUALI\n#\n"

         "# Si applicano test statistici: media, varianza, CHI quadro\n"
         "# Sono stati scelti %d realizzazioni\n"
         "# Vengono stampati su file i dati per\n"
         "# plottare Istogrammi su gnuplot con %d bin\n"
         "# inoltre i dati di RANDU vengono memorizzati in un file\n"
         "# chiamato 'randu.dat' per analizzarlo\n"
         "# visivamente ed osservare i 15 piani:\n#\n#\t\t"
         " x = r_n   y = r_(n+1)   z = r_(n+2).\n#\n"
         "# Sceglire quale generatore analizzare,\n"
         "# in questo codice sono implementati:\n", MAX, BIN);

  int flag, choice, stories=1;

  do{
    printf("# (0) GCL MinSTD, (1) L'ecuyer [64bit], (2) RANDU, (3) rand(),\n"
           "# (4) drand48(), (5) Shift-Register,\n"
           "# Scegliere (6) per eseguirli tutti\n# ");
    scanf("%d", &choice);
  }while(choice < 0 || choice > 6);

  if(choice == 6) stories = 6;

  register ullint i;


  for(flag=0; flag<stories; flag++){ /////////CICLO PER TUTTI GLI ALGORITMI/////////////////////////

    if(choice < 6)flag = choice;


    double ran = 0;
    double sum=0, sum2=0;
    double INV_RANDMAX = 1./RAND_MAX;



    lint *hist;
    hist = (lint *)calloc(BIN, sizeof(lint));
    if(hist == NULL){error(101);}
    int bin;

      /********************************************************************************************/

    if(flag <= 1){

      double invM;
      ullint m, a, seed;

      if(flag == 0){  //GCL - MINIMAL STANDARD

        printf("#\n# GCL - Minimal Standard - FIXED SEED\n");
        m = (1ULL << 31) - 1ULL;
        a = 16807;
        seed = 756431;
      }

      else if(flag == 1){ //L'ecuyer type:1 -> 64bit

        printf("#\n# L'Ecuyer type:1 64bit\n");
        m = ULLONG_MAX;//( 1 << 64 ) - 1;
        a = 1181783497276652981ULL;
        seed = time(NULL);
      }

      else error(99);

      invM = 1./(double)m;
      ullint r = seed % m; //inizializzazione I_0

      begin = clock();

      for(i=0; i<MAX; i++){

        r = ( a * r ) % m;    //il GCL
        ran = (double)r * invM;

        sum += ran;
        sum2 += ran*ran;

        bin = (int)(ran*BIN);
        hist[bin]++;
      }
      end = clock();
    }

      /********************************************************************************************/

      else if(flag == 2){ //GCL - The infamous RANDU

        FILE *output_randu;
        ullint m, a, seed;

        printf("#\n# GCL - RANDU\n");
        output_randu = fopen("randu.dat", "w");
        if (output_randu == NULL){error(100);}

        m = (1ULL << 31);
        a = 65539;
        seed = time(NULL);

        double invM = 1./(double)m;
        ullint r = seed % m; //inizializzazione I_0

        begin = clock();

        for(i=0; i<MAX; i++){

          r = ( a * r ) % m;    //il GCL
          ran = (double)r * invM;

          sum += ran;
          sum2 += ran*ran;

          fprintf(output_randu, "%lf ", ran); //crea anche un file per i piani di randu
          if(i%3 == 2) fprintf(output_randu, "\n");

          bin = (int)(ran*BIN); 
          hist[bin]++;
        }
        end = clock();

        fclose(output_randu);
      }

      /********************************************************************************************/

      else if(flag <= 4){ // rand() e drand48()

        if(flag == 3){

          printf("#\n# rand()\n");

          begin = clock();

          for(i=0; i<MAX; i++){

            ran = rand()*INV_RANDMAX;

            sum += ran;
            sum2 += ran*ran;

            bin = (int)(ran*BIN);
            hist[bin]++;
          }
          end = clock();
        }

        else if(flag == 4){

          printf("#\n# lrand48()\n");

          begin = clock();

          for(i=0; i<MAX; i++){

            ran = lrand48()*INV_RANDMAX;

            sum += ran;
            sum2 += ran*ran;

            bin = (int)(ran*BIN);
            hist[bin]++;
          }
          
          end = clock();
        }
        else error(99);
      }

      /********************************************************************************************/
      else if(flag == 5){ //  SHIFT REGISTER

        printf("#\n# Shift-Register [inizializzato con rand()]\n");
        int addB = 31;
        int addC = 0;

        int BITS = 8 * sizeof(ullint);

        int k;

        ullint *SR, r, THEBIT;

        SR = (ullint *) calloc(SHIFT, sizeof(ullint));
        if(SR == NULL) {error(101);}

        begin = clock();

        for(i=0; i<1000; i++) rand(); //lrand48 gira 1000, inizializzandosi

          for(i=0; i<SHIFT; i++){

            for(k=0; k<BITS; k++){

              ran = (double)rand()*INV_RANDMAX;

              if(ran < 0.5) THEBIT = 1ULL;
              else THEBIT = 0ULL;

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
          }
          end = clock();
          free(SR);
        }

        else error(99);

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
          fprintf(outputH,"%lld %ld\n", (double)(i + 0.5)/BIN, hist[i]);
          phi += hist[i]*hist[i]; //Sfrutto questo ciclo per le somme quadrate necessarie al CHIquadro
        }

      free(hist);
      fclose(outputH);
      //********************************************************************************************

      //Completamento della statistica
      double media = sum/(double)MAX;
      double varianza = (sum2 + (double)MAX * media * media - 2. * media * sum)/(double)(MAX - 1);
      double chi = (double)phi * (double)BIN/(double)MAX - (double)MAX;

      fprintf(stdout, "# Algoritmo:%d\tmedia: %lf, varianza:"
             "%lf, chi su %u GdL: %lf\n", flag, media, varianza, BIN-1, chi);
      fprintf(stdout, "# Execution time:%lf\n", (double)(end-begin)/CLOCKS_PER_SEC);
      

  } //////////////////////////////////FINE CICLO ALGORITMI//////////////////////////////////////////

  exit(0);
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