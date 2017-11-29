#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define NSTORIES 10000
#define NSTEPS 10000
#define NBIN 10

#define A (1.)

typedef unsigned long long int number;
typedef long long int pos;

typedef struct dim{
  double x;
  double y;
} dim;

struct dim init();

int main(){
  register int i;
  int k;

  //L'ecuyer type 1 64bit (maybe)
  number m = (1<<(8*sizeof(number)))-1; //gen the ULLONG_MAX despite the definition
  number a = 1181783497276652981ULL;

  number ran = time(NULL); //seeding
  double inv = 1./(double)m, r;

  dim p;

  dim *somme, *sommeq;
  
  somme = (dim *) calloc(NSTEPS, sizeof(dim));
  if (somme == NULL) { printf("calloc error somme"); exit(-1);}
  sommeq = (dim *) calloc(NSTEPS, sizeof(dim));
  if (sommeq == NULL) { printf("calloc error sommeq"); exit(-1);}


  for (i=0; i<NSTORIES; i++){
    p = init();

    for(k=0; k<NSTEPS; k++){
        
      ran = (a*ran)%m;
      r = (double)ran*inv;

      if (r<0.25)       { p.x+=A; }
      else if (r<0.5)   { p.y+=A; }
      else if (r<0.75)  { p.x-=A; }
      else              { p.y-=A; }

      somme[k].x += p.x;
      somme[k].y += p.y;
      sommeq[k].x += p.x*p.x;
      sommeq[k].y += p.y*p.y;
    }
  }


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
  exit(0);
}

struct dim init(){
  dim a;
  a.x = 0.;
  a.y = 0.;
  return a;
}
