#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NINPUT 1

//RODARI RIVA
//12 ottobre 2017   -   v.0.1.1

struct vector{
  double x; //x = Theta -> angolo
  double v; //v = omega -> vel angolare
};

struct vector verlet(struct vector, double, double, long int);
struct vector velocityverlet(struct vector, double, double);
struct vector RK2(struct vector, double, double, double, double, double, long int);
struct vector RK4(struct vector, double, double, double, double, double, long int);

double phi(double, double, double, double, double, double, double);
double energy(struct vector, double);               

int main(){
  
  int j;
  long int i, steps;                                             
  double x0, v0, omega2, gamma, f0, omegaf, dt, tmax, e, e0;
  
  struct vector state;
  
  FILE *input;
  FILE *output;
  // FILE *order;
  // FILE *poincare;
  // FILE *accumul;
  // FILE *biforc;
  
  input = fopen("input.dat", "r");

  for(j=0; j<NINPUT; j++){

    fscanf(input, "%lf %lf %lf %lf %lf %lf %lf %lf",
	   &x0, &v0, &omega2, &gamma, &f0, &omegaf, &dt, &tmax);
    
    state.x = x0;
    state.v = v0;
    e0 = energy(state, omega2);

    steps = (long int)(tmax/dt);

    output = fopen("data.dat", "w");

    fprintf(output, "%.14lf %.14lf %.14lf %.14lf\n", 0., state.x, state.v, 0.);

    for(i = 0; i<steps; i++){
      
      //state = verlet(state, omega2, dt, i);
      //state = velocityverlet(state, omega2, dt);
      state = RK2(state, omega2, gamma, f0, omegaf, dt, i);

      e = energy(state, omega2);
  
      fprintf(output, "%.14lf %.14lf %.14lf %.14lf\n", dt * (i + 1), state.x, state.v, e/e0 - 1.);
      
    }
    
    fclose(output);

  }

  fclose(input);
  exit(0);
}

struct vector verlet(struct vector old, double omega2, double dt, long int step){

  //Usabile solo se l'accelerazione non dipende dalla velocita'
  
  struct vector new;
  static double nextx;
  
  if(step == 0){
    new.x = old.x + old.v * dt - 0.5 * omega2 * sin(old.x) * dt * dt; //x1
  }
  else {
    new.x = nextx;
  }

  nextx = 2. * new.x - old.x - omega2 * sin(old.x) * dt * dt; //x2  
  new.v = 0.5 * (nextx - old.x)/dt; //v1

  return new;

}

struct vector velocityverlet(struct vector old, double omega2, double dt){
  
  struct vector new;

  new.x = old.x + old.v * dt - 0.5 * omega2 * sin(old.x) * dt * dt;
  new.v = old.v - 0.5 * omega2 * ( sin(old.x) + sin(new.x) ) * dt;

  return new;
}

struct vector RK2(struct vector old, double omega2, double gamma, double f0, double omegaf, double dt, long int step){

  struct vector new;
  double acc;
  acc = phi(old.x, old.v, omega2, gamma, f0, omegaf, (double)dt*step); //phi_n

  new.x = old.x + old.v * dt + 0.5 * acc * dt * dt;
  new.v = old.v + phi(old.x + 0.5 * old.v * dt, old.v + 0.5 * acc * dt, omega2, gamma, f0, omegaf, (double)dt*step + 0.5 * dt)*dt;
  
  return new;
}

struct vector RK4(struct vector old, double omega2, double gamma, double f0, double omegaf, double dt, long int step){

  struct vector new;
  double acc;
  acc = phi(old.x, old.v, omega2, gamma, f0, omegaf, (double)dt*step); //phi_n

  new.x = old.x + old.v * dt + 0.5 * acc * dt * dt;
  new.v = old.v + phi(old.x + 0.5 * old.v * dt, old.v + 0.5 * acc * dt, omega2, gamma, f0, omegaf, (double)dt*step + 0.5 * dt)*dt;
  
  return new;
}

double phi(double x, double v, double omega2, double gamma, double f0, double omegaf, double t) {

  return -omega2 * sin(x) -gamma * v + f0 * cos(omegaf * t);
}

double energy(struct vector state, double omega2) {

  return 0.5 * state.v * state.v + omega2 * (1 - cos(state.x));
}







//Eulero & Eulero Cromer Folded Algorithms


/* struct vector euler(struct vector old, double dt, double phi){    //Eulero

  struct vector new;new.a = old.a + old.va * dt;
  new.va = old.va + phi * dt;

  return new;
}


struct vector cromer(struct vector old, double dt, double phi) {    //Eulero-Cromer

  struct vector new;
  new.va = old.va + phi * dt;
  new.a = old.a + new.va * dt;

  return new;
  
} */