#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


#define L (NSTEPS*DIM)
#define NSTORIES 1
#define NSTEPS 1000
#define NBIN 10
#define DIM 2

#define A (1.)

typedef unsigned long long int number;
typedef long long int pos;

typedef struct dim{
  double x;
  double y;
} dim;

struct dim init();

int main(){

  printf("porcoddio\n");

  int k, i;

  srand(time(NULL));

  //L'ecuyer type 1 64bit (maybe)
  //number m = (1<<(8*sizeof(number)))-1; //gen the ULLONG_MAX despite the definition 64bit
  //number a = 1181783497276652981ULL;

  //number ran = time(NULL); //seeding
  //double inv = 1./(double)m, r;

  number ran;

  double  r, inv = 1./((double)RAND_MAX); 

  dim p; //p è dove si trova

  //dim *somme, *sommeq;
  
  int M[L][L];

  // somme = (dim *) calloc(NSTEPS, sizeof(dim));
  // if (somme == NULL) { printf("calloc error somme"); exit(-1);}
  // sommeq = (dim *) calloc(NSTEPS, sizeof(dim));
  // if (sommeq == NULL) { printf("calloc error sommeq"); exit(-1);}

  int x=NSTEPS, y=NSTEPS;

  for (i=0; i<NSTORIES; i++){
    p = init();

    for(k=0; k<NSTEPS; k++){
      
      if ( M[x+1][y] > 0 && M[x-1][y] > 0 && 
           M[x][y+1] > 0 && M[x][y+1] > 0 ){
        k = NSTEPS;
        printf("intrappolato storia:%d\n", i);
      }

      else{
        ran = rand();
        r = (double)ran*inv;

        x = p.x + NSTEPS;
        y = p.y + NSTEPS;

        if (r<0.25 && M[x+1][y] == 0) { p.x+=A; }
        else if (r<0.5 && M[x][y+1]) { p.y+=A; }
        else if (r<0.75 && M[x-1][y] == 0) { p.x-=A; }
        else if (r<1. && M[x][y-1] == 0) { p.y-=A; }

        x = p.x + NSTEPS;
        y = p.y + NSTEPS;

        M[x][y] = 1;

        // somme[k].x += p.x;
        // somme[k].y += p.y;
        // sommeq[k].x += p.x*p.x;
        // sommeq[k].y += p.y*p.y;

      }
    }
  }

  FILE *output2;
  output2 = fopen("walkpath.dat", "w");

  for(x=0; x<L; x++){
    for(y=0; y<L; y++){
      if(M[x][y] == 1)fprintf(output2, "%d %d\n", x, y);
    }
  }

  fclose(output2);

/*
  double iNSTORIES = 1./(double)NSTORIES;

  FILE *output1;
  output1 = fopen("walkmean.dat", "w");

  for(i=0; i<NSTEPS; i++){
    somme[i].x *= iNSTORIES;
    somme[i].y *= iNSTORIES;
    sommeq[i].x *= iNSTORIES;
    sommeq[i].y *= iNSTORIES;
    fprintf(output1, "%d %lf %lf %lf %lf\n", i, somme[i].x, somme[i].y, sommeq[i].x, sommeq[i].y);
  }

  fclose(output1);
  free(somme);
  free(sommeq);
  //system("gnuplot < baleator.gp");
*/

  exit(0);
}

struct dim init(){
  dim a;
  a.x = 0.;
  a.y = 0.;
  return a;
}
