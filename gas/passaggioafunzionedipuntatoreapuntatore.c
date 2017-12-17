#include <stdio.h>
#include <stdlib.h>


void init(int**);

int main(){

  int **site;
  int i, k;
  site = (int**) calloc(20, sizeof(int*));
  for(i=0; i<20; i++){
    site[i] = (int *) calloc(20, sizeof(int));
  }

  init(site);

  for(i=0; i<20; i++){

    for(k=0; k<20; k++){
      fprintf(stdout,"%d ",site[i][k]);
    }
    fprintf(stdout, "\n");
  }  

  exit(0);
}

void init(int **site){

  int i, k;
  for(i=0; i<20; i++){
    for(k=0; k<20; k++){
      site[i][k] = i + k;
    }
  }
}