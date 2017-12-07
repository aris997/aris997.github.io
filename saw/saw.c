#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define NSTORIES 10000
#define NSTEPS 1000
#define L 2001
#define DIM 2

#define A 1

typedef struct dim{
  int x;
  int y;
} dim;

struct dim init();
dim copy(dim);
int stuck(dim , dim *, int);
int check(dim , dim *, int);

int main(){
  clock_t begin = clock();
  srand(time(NULL));

  int i, k, mov, storie=0, gino[NSTEPS]={0};
  dim p, newp, *visit;
  double  r, inv = 1./(double)RAND_MAX, media=0; 

  for (i=0; i<NSTORIES; i++){

    p = init();
    mov = 0;

    visit = (dim *) calloc(NSTEPS + 1, sizeof(dim));
    if(visit == NULL){printf("ERROR CALLOC\n"); exit(0);}
    
    for(k = 0; k<NSTEPS; k++){
      
      newp = copy(p);

      r = (double) rand() * inv;

      if (r < 0.25) { newp.x += A; }
      else if (r<0.5) { newp.x -= A; }
      else if (r<0.75) { newp.y += A; }
      else if (r<=1) { newp.y -= A; }
      else { printf("Error!\n"); exit(-1);}

      if(check(newp, visit, mov) == 0){
	//printf("%d %d \n", newp.x, newp.y);
       p = copy(newp);
       mov++;
       visit[mov] = copy(p);
     }

     if(stuck(p, visit, mov) >= 4){
	//printf("La storia %d si conclude allo step %d \n", i+1, k+1);
       media+=mov;
       storie++;
       gino[k]++;
       k = NSTEPS;
     }
   }
   
   free(visit);
 }

 int ginohelper;

 FILE *output1;
 output1 = fopen("gino.dat", "w");
 
 for(i=0; i<NSTEPS; i++){
  ginohelper=0;
  for(k=0; k<=i; k++){
    ginohelper+=gino[k];
  }
  fprintf(output1, "%d %lf\n", i, (double)ginohelper/NSTORIES);
}

printf("Il punto finale medio e' %lf, %d storie su %d\n", media/(double)storie, storie, NSTORIES);

fclose(output1);

clock_t end = clock();
printf("execution time:%lf\n", (double)(end-begin)/CLOCKS_PER_SEC);
exit(0);
}

int stuck(dim pos, dim visit[], int step){
  
  int N = 0, i;

  for(i=0; i<step; i++){
    if(pos.x == visit[i].x - 1 && pos.y == visit[i].y) N++;
    else if(pos.x == visit[i].x + 1 && pos.y == visit[i].y) N++;
    else if(pos.x == visit[i].x && pos.y == visit[i].y + 1) N++;
    else if(pos.x == visit[i].x && pos.y == visit[i].y - 1) N++;
  }

  return N;

}

int check(dim pos, dim visit[], int step){
  
  int i;

  for(i=0; i<step; i++){
    if(pos.x == visit[i].x && pos.y == visit[i].y) return 1;
  }

  return 0;
  
}

dim copy(dim old){
  dim a;
  a.x = old.x;
  a.y = old.y;
  return a;
}

struct dim init(){
  dim a;
  a.x = 0;
  a.y = 0;
  return a;
}
