#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MY_EXIT_SUCCESS       0
#define MY_EXIT_FAILURE      -9
#define MY_EMPTY        9999999

#define L 20
#define NUM_SWEEPS 1000
#define NUM_SAMPLES 5000
#define NUM_MEASUREMENTS 100
#define MEASUREMENT_PERIOD (NUM_SWEEPS/NUM_MEASUREMENTS)
#define DIM 2
#define VOLUME (L * L)

#define MY_DEBUG

typedef unsigned long long RANDOM_TYPE;
RANDOM_TYPE a, seed, randomNumber;
double doubleM, iDoubleM;

long numberOfSweeps = NUM_SWEEPS, numberOfSamples = NUM_SAMPLES;
long particleOfSite[L][L];
long plusNeighbor[L];
long minusNeighbor[L];
long positionOfParticle[VOLUME][DIM];
long truePositionOfParticle[VOLUME][DIM];
long zeroPositionOfParticle[VOLUME][DIM];
long particleNumber;
double averageDeltaR2[NUM_MEASUREMENTS];
double errorDeltaR2[NUM_MEASUREMENTS];

FILE *fDat;

double myInit(void);
long initLattice(double rho);
void updateLattice(long thisNumberOfParticles);
double measure(long thisNumberOfParticles);
void myEnd(double averageRho);

/*********************************/
int main(void) {

  long sample, sweep, thisN;
  double rho, averageRho = 0.0;

  rho = myInit();

  for(sample = 0; sample < numberOfSamples; sample++) {
    long numMeasure = 0;
    thisN = initLattice(rho);
    averageRho += (double)thisN / (double)VOLUME;
    for(sweep = 0; sweep < numberOfSweeps; sweep++) {
      updateLattice(thisN);
      if(((1+sweep)%MEASUREMENT_PERIOD)==0) {
        double deltaR2;
        deltaR2 = measure(thisN);
        #ifdef MY_DEBUG
        if(numMeasure>=NUM_MEASUREMENTS) {
          printf("Errore: numMeasure e' troppo grande\n");
          exit(MY_EXIT_FAILURE);
        }
        #endif
        averageDeltaR2[numMeasure] += deltaR2;
        errorDeltaR2[numMeasure] += (deltaR2 * deltaR2);
        numMeasure++;
      }
    }
  }
  myEnd(averageRho);
}

/*********************************/
double myInit(void) {
  long i;
  double rho;

  fDat = fopen("lattice_gas.dat","w");
  if(fDat == NULL) {
    printf("Errore: non riesco ad aprire il file di scrittura\n");
    exit(MY_EXIT_FAILURE);
  }

  a = 1181783497276652981;
  seed = 131419;

  doubleM = pow(2.,64.);
  iDoubleM = 1.0 / doubleM;

  printf("# a = %Lu m = 2^64 I0 = %Lu\n", a, seed);
  printf("# Numero di passi temporali: %d\n", numberOfSweeps);
  printf("# Numero di campioni: %d\n", numberOfSamples);
  printf("# Numero di misure: %d\n", NUM_MEASUREMENTS);
  printf("# Periodo delle misure: %d\n", MEASUREMENT_PERIOD);
  if((NUM_MEASUREMENTS*MEASUREMENT_PERIOD)!=numberOfSweeps) {
    printf("Errore: numero di passi non multiplo di numero di misure\n");
    exit(MY_EXIT_FAILURE);
  }
  fprintf(fDat, "# a = %Lu m = 2^64 I0 = %Lu\n", a, seed);
  fprintf(fDat, "# Numero di passi temporali: %d\n",
  numberOfSweeps);
  fprintf(fDat, "# Numero di campioni: %d\n", numberOfSamples);
  fprintf(fDat, "# Numero di misure: %d\n", NUM_MEASUREMENTS);
  fprintf(fDat, "# Periodo delle misure: %d\n", MEASUREMENT_PERIOD);

  #ifdef MY_DEBUG
  printf("# Debug mode attivo: l'esecuzione e` rallentata.\n");
  fprintf(fDat, "# Debug mode attivo: l'esecuzione e` rallentata.\n");
  #endif
  if(sizeof(RANDOM_TYPE)<8) {
    printf("Errore: RANDOM_TYPE deve avere almeno 8 byte,"
    " mentre ne ha  %d\n", sizeof(RANDOM_TYPE));
    exit(MY_EXIT_FAILURE);
  }
  if(DIM!=2) {
    printf("Errore: questo programma e' stato verificato,"
    " solo in D = %d\n", DIM);
    exit(MY_EXIT_FAILURE);
  }

  randomNumber = seed;

  for(i = 0; i < L; i++) {
    plusNeighbor[i] = i+1;
  }
  plusNeighbor[L-1] = 0;
  for(i = 0; i < L; i++) {
    minusNeighbor[i] = i-1;
  }
  minusNeighbor[0] = L-1;

  printf("# inserire il valore della densita':\n");
  scanf("%lg", &rho);
  particleNumber = (long) (rho * (double)VOLUME);
  printf("# densita' attesa = %lg numero medio = %ld\n",
  rho, particleNumber);
  fprintf(fDat, "# densita' attesa = %lg numero medio = %ld\n",
  rho, particleNumber);

  if(rho>=1.0) {
    printf("Errore: rho troppo grande %lg\n", rho);
    exit(MY_EXIT_FAILURE);
  }
  for(i = 0; i < NUM_MEASUREMENTS; i++) {
    averageDeltaR2[i] = 0.0;
    errorDeltaR2[i] = 0.0;
  }
  return rho;
}

/*********************************/
void myEnd(double averageRho) {
  long i;

  averageRho /= (double)NUM_SAMPLES;
  printf("# densita media = %lg\n", averageRho);
  fprintf(fDat, "# densita media = %lg\n", averageRho);

  for(i = 0; i < NUM_MEASUREMENTS; i++) {
    averageDeltaR2[i] /= (double)NUM_SAMPLES;
    errorDeltaR2[i] /= (double)NUM_SAMPLES;
    errorDeltaR2[i] -=  ( averageDeltaR2[i] * averageDeltaR2[i] );
    if(NUM_SAMPLES>1) {
      errorDeltaR2[i] /= ((double)NUM_SAMPLES - 1);
    } else{
      errorDeltaR2[i] = 0.0;
    }
    if(errorDeltaR2[i] > 0.0) {
      errorDeltaR2[i] = sqrt(errorDeltaR2[i]);
    }else{
      errorDeltaR2[i] = 0.0;
    }

    fprintf(fDat,"%ld %lg %lg\n",
    i*MEASUREMENT_PERIOD+MEASUREMENT_PERIOD-1,
    averageDeltaR2[i], errorDeltaR2[i]);
  }
  fclose(fDat);
}

/*********************************/
long initLattice(double rho) {
  long x, y, thisN;
  double r, thisRho;

  for(x = 0; x < L; x++) {
    for(y = 0; y < L; y++) {
      particleOfSite[x][y] = MY_EMPTY;
      thisN = x + y * L;
      positionOfParticle[thisN][0] = MY_EMPTY;
      positionOfParticle[thisN][1] = MY_EMPTY;
      truePositionOfParticle[thisN][0] = MY_EMPTY;
      truePositionOfParticle[thisN][1] = MY_EMPTY;
      zeroPositionOfParticle[thisN][0] = MY_EMPTY;
      zeroPositionOfParticle[thisN][1] = MY_EMPTY;
    }
  }

  thisN = 0;
  for(x = 0; x < L; x++) {
    for(y = 0; y < L; y++) {
      randomNumber = a * randomNumber;
      r = (double)randomNumber * iDoubleM;
      if(r < rho) {
        particleOfSite[x][y] = thisN;
        positionOfParticle[thisN][0] = x;
        positionOfParticle[thisN][1] = y;
        zeroPositionOfParticle[thisN][0] = x;
        zeroPositionOfParticle[thisN][1] = y;
        truePositionOfParticle[thisN][0] = x;
        truePositionOfParticle[thisN][1] = y;
        thisN++;
      }
    }
  }
  return thisN;
}

/*********************************/
void updateLattice(long thisN) {
  long number, thisParticle, this_direction;
  long nX, nY, x, y, trueX, trueY;
  double r;

  /* un passo e' fatto da rho * N tentativi (usiamo il rho medio) */
  for(number = 0; number < particleNumber; number++) {
    randomNumber = a * randomNumber;
    r = (double)randomNumber * iDoubleM;
    thisParticle = (long)(r * (double)thisN);
    if(thisParticle>=thisN) thisParticle = thisN-1;
    randomNumber = a * randomNumber;
    r = (double)randomNumber * iDoubleM;
    this_direction = (long)(r * (double)(2*DIM));
    if(this_direction>=(2*DIM)) {
      printf("large direction\n");
      this_direction = 2*DIM-1;
    }
    x = positionOfParticle[thisParticle][0];
    y = positionOfParticle[thisParticle][1];
    trueX = positionOfParticle[thisParticle][0];
    trueY = positionOfParticle[thisParticle][1];
    #ifdef MY_DEBUG
    if( (x >= L) || ( y>= L)
    || (trueX == MY_EMPTY) || (trueY == MY_EMPTY) ) {
      printf("Errore interno: grande posizione\n");
      exit(MY_EXIT_FAILURE);
    }
    if(particleOfSite[x][y] != thisParticle) {
      printf("Errore interno: particella indecisa x %ld y %ld P %ld TP %ld\n",
      x, y, particleOfSite[x][y], thisParticle);
      exit(MY_EXIT_FAILURE);
    }
    #endif
    if(this_direction == 0) {
      nX = plusNeighbor[x];
      nY = y;
    } else if(this_direction == 1) {
      nX = minusNeighbor[x];
      nY = y;
    } else if(this_direction == 2) {
      nX = x;
      nY = plusNeighbor[y];
    } else if(this_direction == 3) {
      nX = x;
      nY = minusNeighbor[y];
    } else{
      printf("Errore interno: cattivi vicini\n");
      exit(MY_EXIT_FAILURE);
    }
    if(particleOfSite[nX][nY] == MY_EMPTY) {/* qui cambiamo */
      particleOfSite[nX][nY] = particleOfSite[x][y];
      particleOfSite[x][y] = MY_EMPTY;
      positionOfParticle[thisParticle][0] = nX;
      positionOfParticle[thisParticle][1] = nY;
      if(this_direction==0) {
        truePositionOfParticle[thisParticle][0]++;
      } else if(this_direction==1) {
        truePositionOfParticle[thisParticle][0]--;
      } else if(this_direction==2) {
        truePositionOfParticle[thisParticle][1]++;
      } else if(this_direction==3) {
        truePositionOfParticle[thisParticle][1]--;
      }
    }
  }
}


/*********************************/
double measure(long thisN) {
  long i;
  double r2 = 0.0;

  for(i = 0; i < thisN; i++) {
    double d0 =
    truePositionOfParticle[i][0] - zeroPositionOfParticle[i][0];
    double d1 =
    truePositionOfParticle[i][1] - zeroPositionOfParticle[i][1];
    r2 += d0 * d0 + d1 * d1;
  }
  return r2 / (double)thisN;
}
