#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define SHIFT 1000
#define NMAX 10000
#define NBIN 10


typedef unsigned long long int RANDOM;


int main(int argc, char **argv){
  clock_t begin = clock();

  srand48(time(NULL));

  RANDOM a, b, m, seed, r; //variabili
  long int i, j;  //contatori
  long int hist[NBIN] = {0}, phi=0;
  int bin;
  int chooser;

  RANDOM  SR[SHIFT], THEBIT; //sr
  int addB, addC;

  double sumr = 0, sumr2 = 0, media, varianza, number, chi;
  double x, y, z;
  FILE *output1, *output2;

  seed = time(NULL);
  
  output1 = fopen("hist.dat", "w");
  output2 = fopen("planes.dat", "w");

  if (argc > 1) chooser = atoi(argv[1]);
  else {
    chooser = 0;
  }

  printf("# Insert by command line\n# "
         "(0) Minimal Standard (as default),\n# "
         "(1) for Shift Register or\n# "
         "(2) RANDU\n# "
         "(3) rand()\n# "
         "(4) lrand48()\n");  


  /***********************************************************************************************/
  if (chooser == 0) {

    //MINIMAL STANDARD
    a = 16807;
    b = 0;
    m = (1ULL << 31) - 1ULL;


    r = seed % m; //Inizializzazione di I_0

    //GENERATORE GCL

    for (i=0; i<NMAX; i++){
      r = (a*r + b) % m;
      number = (double)r / (double)m;
      sumr += number;
      sumr2 += number*number;
      //printf("%lf\n",number);

      bin = (int)(number*NBIN);
      hist[bin]++;

      if (i%3 == 0) x = number;
      else if (i%3 == 1) y = number;
      else if (i%3 == 2) {
        z = number;
        fprintf(output2, "%lf %lf %lf\n", x, y, z);
      }
    }

    //DATI PER ISTOGRAMMA
    for(i=0; i<NBIN; i++){
      fprintf(output1,"%ld %ld\n", i, hist[i]);
      phi += hist[i]*hist[i];
    }
  }

  /***********************************************************************************************/
  //SHIFT REGISTER
  else if (chooser == 1) {

    addB = 31;
    addC = 0;

    for (i=0; i<1000; i++) lrand48(); 

    for (i=0; i<SHIFT; i++) SR[i]=0;

    for (i=0; i<SHIFT; i++){
      for (j=0; j<8*sizeof(RANDOM); j++){
   number = (double)lrand48()/(double)RAND_MAX;
   if (number < 0.5) {
     THEBIT = 1;
   }
   else {
     THEBIT = 0;
   }
   SR[i] = SR[i] | (THEBIT << j);
      }
    }

    //GENERAZIONE

    for (i=0; i<NMAX; i++){
      r = SR[addB] ^ SR[addC];

      SR[addC] = r;
      addC++;
      addB++;
      if (addB == SHIFT) addB = 0;
      if (addC == SHIFT) addC = 0;

      number = (double)r / (double)ULLONG_MAX;

      sumr += number;
      sumr2 += number*number;
      //printf("%lf\n",number);

      bin = (int)(number*NBIN);
      hist[bin]++;

      if (i%3 == 0) x = number;
      else if (i%3 == 1) y = number;
      else if (i%3 == 2) {
   z = number;
   fprintf(output2, "%lf %lf %lf\n", x, y, z);
      }
    }



    //DATI PER ISTOGRAMMA
    for(i=0; i<NBIN; i++){
      fprintf(output1,"%ld %ld\n", i, hist[i]);
      phi += hist[i]*hist[i];
    }
  }

  /*************************************************************************************************/
  //RANDU
  if(chooser == 2){

    printf("# Randu\n");

    a = 65539;
    b = 0;
    m = (1ULL << 31);



    r = seed % m; //Inizializzazione di I_0

    //GENERATORE GCL

    for (i=0; i<NMAX; i++){
      r = (a*r + b) % m;
      number = (double)r / (double)m;
      sumr += number;
      sumr2 += number*number;
      //printf("%lf\n",number);

      bin = (int)(number*NBIN);
      hist[bin]++;

      if (i%3 == 0) x = number;
      else if (i%3 == 1) y = number;
      else if (i%3 == 2) {
        z = number;
        fprintf(output2, "%lf %lf %lf\n", x, y, z);
      }
    }

    //DATI PER ISTOGRAMMA
    for(i=0; i<NBIN; i++){
      fprintf(output1,"%ld %ld\n", i, hist[i]);
      phi += hist[i]*hist[i];
    }
  }

  /*************************************************************************************************/

  //rand()

  if(chooser == 3){

    //rand()




  }

  

  //METODI STATISTICI
  media = sumr/(double)NMAX;
  varianza = (sumr2 + (double)NMAX * media * media - 2. * media * sumr)/(double)(NMAX - 1);
  chi = (double)phi * (double)NBIN/(double)NMAX - (double)NMAX;


  printf("# media: %lf varianza: %lf chi su %d GDL: %lf\n", media, varianza, NBIN-1, chi);




  fclose(output1);
  fclose(output2);

#ifdef CRONO
  clock_t end = clock();
  double execution_time = (double)(end - begin)/CLOCKS_PER_SEC;
  printf("# Execution time: %lf\n", execution_time);
#endif

  exit(0);
}
