#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define dt 0.0005
double input(char* stringa);

int main()
{
  
  double omega2;
  omega2=input("omega2");
  // int N=2*sqrt(4*pow(M_PI,2)/(omega2))/dt;
  int N;
  printf("inserisci N\n");
  scanf("%d",&N);
  double x[N],v[N],phi[N],v0,x0; //declaring variables space,velocity, energy
  x[0]=input("posizione iniziale");
  v[0]=input("velocita' iniziale");
  double teta=atan(-v[0]/(sqrt(omega2)*x[0]));
  double A;
  if(v[0]==0)A=x[0]/cos(teta);
  if(x[0]==0)A=v[0]/(sqrt(omega2)*sin(teta));
  int n=0;
  double t0=0;
  double t;
  int scelta,k=0;
  printf("\nPremi 0 se vuoi usare Eulero,1 se vuoi usare Eulero-Cromer\n");
  scanf("%d",&scelta);
  if(scelta==1)
    { 
      k=1;
    } else if(scelta==0)k=0;
  for(n=0;n<N;n++)
	{
	  phi[n]=-omega2*x[n];
	  v[n+1]=v[n]+phi[n]*dt;
	  x[n+1]=x[n]+v[n+k]*dt;
    }
  printf("\nSpazio(x)\tVelocita'(v)\tSpazio(xanalitico\tVelocita'(v)analitico");
  for(n=0;n<N;n++)
    printf("\n%lf\t%lf\t%lf\t%lf",x[n],v[n],-A*cos(sqrt(omega2)*(t0+n*dt)+teta),sqrt(omega2)*A*sin(sqrt(omega2)*(t0+n*dt)+teta));
  printf("\n");
  FILE *fp;
  int i=0;
  fp=fopen("sfasi.dat","w");
  for(i=0;i<N;i++)
    {
      fprintf(fp,"%lf\t%lf\n",x[i],v[i]);
    }
  double E[N];
  double m=1;
  for(n=0;n<N;n++)
    {
      E[n]=0.5*m*pow(v[n],2)+0.5*omega2*m*pow(x[n],2);
    } 
  FILE *pf;
  pf=fopen("energia.dat","w");
  for(i=0;i<N;i++)
    {
      fprintf(pf,"%lf\t%lf\n",t0+i*dt,E[i]);
    }

   FILE *u;
  u=fopen("var_energia.dat","w");
  for(i=0;i<N;i++)
    {
      fprintf(u,"%lf\t%lf\n",t0+i*dt,(E[i]-E[0])/E[0]);
    }
  


  return 0;  
}

double input(char* stringa)
{
  double value;
  printf("Inserisci %s\n",stringa);
  scanf("%lf",&value);
  return value;
}
