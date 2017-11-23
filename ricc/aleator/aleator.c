#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define NSTORIES 1000
#define NSTEPS 1000
#define NBIN 10

typedef unsigned long long int number;
typedef long long int pos;

int main () {

  long int i, k;
  double r, *media, *mediaq, invRANDMAX = 1./RAND_MAX;
  pos x, xmin = 0, xmax = 0, *posizione;
  FILE *output1, *output2;
  //number m, a;

  //l_ecuyer
  /*m = pow(2, 64);
    a = 1181783497276652981ULL; 
  */
    
  media =(double *) calloc(NSTEPS, sizeof(double));
  if (media == NULL) { printf("calloc error media"); exit(-1);}
  mediaq = (double *) calloc(NSTEPS, sizeof(double));
  if (mediaq == NULL) { printf("calloc error mediaq"); exit(-1);}
  posizione = (pos *) calloc(NSTORIES, sizeof(pos));
  if (posizione == NULL) { printf("calloc posizione"); exit(-1);}

  srand48(time(NULL));



  for (k=0; k<NSTORIES; k++) {
    x = 0; 

    for (i=0; i<NSTEPS; i++) {
      r = (double)lrand48()*invRANDMAX;
        
      if (r < 0.5) x += 1;
      else x -= 1;
      media[i] += x;
      mediaq[i] += x*x;
    }

    posizione[k]=x;
    if (x < xmin) xmin = x;
    else if (x > xmax) xmax = x;
  }

  output1 = fopen("media.dat", "w");

  for (i=0; i<NSTEPS; i++){
    media[i] /= NSTORIES;
    mediaq[i] /= NSTORIES;
    fprintf(output1, "%ld %lf %lf\n", i+1, media[i], mediaq[i]);
  }

  fclose(output1);

/******HISTOGRAMMING******/
  int delta = (xmax - xmin);
  double wbin = (double)NBIN/delta;
  int hist[NBIN]={0};
  int address;
  printf("xmin %lld xmax %lld wbin: %lf\n", xmin, xmax, wbin);
  
  for (i=0; i<NSTORIES; i++) {
    address = (posizione[i]-xmin)*wbin;
    hist[address]++;
  }

  output2 = fopen("posi.dat", "w");
  for (i=0; i<NBIN; i++) fprintf(output2, "%ld %d\n", i+1, hist[i]);
  #ifdef PLOT
    system("gnuplot < gnu.gp");
  #endif

  fclose(output2);
  free(media);
  free(mediaq);
  free(posizione);

  exit(0);
}
