#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NINPUT 1

//RODARI RIVA
//12 ottobre 2017   -   v.0.1.1

typedef struct vector{
  double x; //x = Theta -> angolo
  double v; //v = omega -> vel angolare
} vector;


typedef struct par{
  double omega2;  //omega^2 
  double gamma;   //gamma
  double f0;
  double omegaf;
  double dt;      //tempo di integrazione
} par;

typedef long int counter;

struct vector verlet(vector, par, counter);
struct vector velocityverlet(vector, par, counter); //per simmetria di codice passo counter anche se a v.v. non serve
struct vector RK2(vector, par, counter);
struct vector RK4(vector, par, counter);

double phi(double, double, par, double);
double energy(vector, par);

int main(int argc, char *argv[]) {

  int j, selection;
  counter i, steps;
  double x0, v0, tmax, e, e0;

  vector state;  //creo una struct per state(x,v) e var per parametri
  par var;

  FILE *input;
  FILE *output;
  // FILE *order;
  // FILE *poincare;
  // FILE *accumul;
  // FILE *biforc;


	printf("Default settings use RK4 to integrate\n");
  
  if (argc == 2) {	 // selection value (0) verlet, (1) velocity verlet, (2) RK2, (4) RK4
  	if (atof(argv[2]) == 0) selection = 0;
  	else if (atof(argv[2]) == 1) selection = 1;
  	else if (atof(argv[2]) == 2) selection = 2;
  	else selection = 4;
  }
  else if (argc > 2) printf("too many arguments\n");
  else selection = 4;


  char filename[20];
  input = fopen("input.dat", "r");

  for(j=0; j<NINPUT; j++) {

    fscanf(input, "%lf %lf %lf %lf %lf %lf %lf %lf", &x0, &v0, &var.omega2, &var.gamma, &var.f0, &var.omegaf, &var.dt, &tmax);
    
    state.x = x0;
    state.v = v0;

    e0 = energy(state, var);

    steps = (long int)(tmax/var.dt);

    sprintf(filename, "data.dat");  //E' possibile resettare un char richiamandolo in una funzione sprintf
    output = fopen(filename, "w");


    fprintf(output, "%.8lf %.16lf %.16lf %.24lf\n", 0., state.x, state.v, 0.);

	  vector (*algorithm)(vector, par, counter);

         if (selection == 0) algorithm = &verlet;
    else if (selection == 1) algorithm = &velocityverlet;
    else if (selection == 2) algorithm = &RK2;
    else if (selection == 4) algorithm = &RK4;
    else exit(EXIT_FAILURE);


    for (i=0; i<steps; i++) {

      state = algorithm(state, var, i);
      e = energy(state, var);
      fprintf(output, "%.8lf %.16lf %.16lf %.24lf\n", var.dt * (i + 1), state.x, state.v, e/e0 - 1.);      
    }
    
    fclose(output);
  }

  // GNUPLOT implementation
  char *StringheGNUPLOT[] = {"gnuplot", "plot 'data.dat' u 1:2 w l", "plot 'data.dat' u 2:3 w l", "plot 'data.dat' u 1:4 w l"};
  
  system(*StringheGNUPLOT);  


  fclose(input);
  exit(0);
}

struct vector verlet(vector old, par var, counter step) {

  //Usabile solo se l'accelerazione non dipende dalla velocita'

  struct vector new;
  static double nextx;
  
  if(step == 0) {
  	new.x = old.x + old.v * var.dt - 0.5 * var.omega2 * sin(old.x) * var.dt * var.dt; //x1
  }
  
  else {
    new.x = nextx;
  }

  nextx = 2. * new.x - old.x - var.omega2 * sin(old.x) * var.dt * var.dt; //x2  
  new.v = 0.5 * (nextx - old.x)/var.dt; //v1

  return new;
}


struct vector velocityverlet(vector old, par var, counter step) {

  struct vector new;

  new.x = old.x + old.v * var.dt - 0.5 * var.omega2 * sin(old.x) * var.dt * var.dt;
  new.v = old.v - 0.5 * var.omega2 * ( sin(old.x) + sin(new.x) ) * var.dt;

  return new;
}


struct vector RK2(vector old, par var, counter step) {

  struct vector new;
  double acc;
  acc = phi(old.x, old.v, var, (double)var.dt*step); //phi_n

  new.x = old.x + old.v * var.dt + 0.5 * acc * var.dt * var.dt;
  new.v = old.v + phi(old.x + 0.5 * old.v * var.dt, old.v + 0.5 * acc * var.dt, var, (double)var.dt*step + 0.5 * var.dt)*var.dt;
  
  return new;
}


struct vector RK4(vector n, par var, counter step) {

  double X[5], V[5]; //definisco array di 5 celle solo per avere una corrispondenza con la teoria
  double t, dt;

  dt = var.dt;
  t = (double)var.dt*step;

  X[1] = n.v * dt;
  V[1] = phi(n.x, n.v, var, t) * dt;
  X[2] = (n.v + V[1]/2.) * dt;
  V[2] = phi(n.x + X[1]/2., n.v + V[1]/2., var, t + dt/2.) * dt;
  X[3] = (n.v + V[2]/2.) * dt;
  V[3] = phi(n.x + X[2]/2., n.v + V[2]/2., var, t + dt/2.) * dt;
  X[4] = (n.v + V[3]) * dt;
  V[4] = phi(n.x + X[3], n.v + V[3], var, t + dt) * dt;

  n.x += (X[1] + 2. * X[2] + 2. * X[3] + X[4])/6.;
  n.v += (V[1] + 2. * V[2] + 2. * V[3] + V[4])/6.;
  
  return n;
}


double phi(double x, double v, par var, double t) {

  return -var.omega2 * sin(x) -var.gamma * v + var.f0 * cos(var.omegaf * t);
}


double energy(vector state, par var) {

  return 0.5 * state.v * state.v + var.omega2 * (1 - cos(state.x));
}
