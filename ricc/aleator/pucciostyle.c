#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MAXSTRING 100
#define EPS 0.01
#define INCREMENTO 0.00001
#define DTMAX 0.1

double dInput (char* nome);
double fx(double a, double b, double rho,double x0,double y0,double z0);
double fy(double a, double b, double rho,double x0,double y0,double z0);
double fz(double a, double b, double rho,double x0,double y0,double z0);
double RK4(double a,double b,double rho, double x0, double y0, double z0, long int N, double dt);
void RK4b(double a,double b,double rho, double x0, double y0, double z0, long int N,double dt);
void banner();
void start_time(void);
double prn_time(void);

typedef struct
{
  clock_t begin_clock, save_clock;
  time_t begin_time,save_time;
} time_keeper;

static time_keeper tk;

int main()
{
  banner();
  FILE *v;
  v=fopen("c.dat","w+");
  double a,b,c,rho,dt,x0,y0,z0,tmax;
  unsigned int N;
  //INPUT DEI PARAMETRI
  a=dInput("parametro a");
  b=dInput("parametro b");
  rho=dInput("parametro rho");
  //INPUT DEL PASSO D'INTEGRAZIONE E DEL TEMPO D'INTEGRAZIONE 
  dt=dInput("passo d'integrazione");
  tmax=dInput("tempo d'integrazione");
  //ALLOCAZIONE DINAMICA DELLA MEMORIA PER LA STRINGA CHE CONTIENE GLI ELEMENTI PER STUDIARE L'ERRORE D'INTEGRAZIONE
  char* ps2;
  ps2=malloc(40*((DTMAX-dt)/INCREMENTO));
  memset(ps2,0x00,40*((DTMAX-dt)/INCREMENTO));
  char *ps2b=ps2;
  //INPUT DELLE CONDIZIONI INIZIALI
  x0=dInput("condizione iniziale x0");
  y0=dInput("condizione iniziale y0");
  z0=dInput("condizione iniziale z0");  
  double c0=log(x0)-x0+log(y0)-y0;
  N=(unsigned int)(tmax/dt); //CALCOLO DEL NUMERO DI PASSI D'INTEGRAZIONE
  //Inizio a cronometrare il codice
  start_time();
  RK4b(a,b,rho,x0,y0,z0,N,dt);
  //Stampa del tempo d'esecuzione
  prn_time();

  //ERRORE D'INTEGRAZIONE
  start_time();
  while (dt<DTMAX)
    {
      dt+=INCREMENTO;
      c=RK4(a,b,rho,x0,y0,z0,N,dt);
      sprintf(ps2b,"%1.14lf\t%1.14lf\t\n",dt,fabs((c/c0)-1));
      ps2b+=strlen(ps2b);
    }
  //FINE CICLO ERRORE D'INTEGRAZIONE
  fprintf(v,"%s",ps2);
  free(ps2);
  printf("\n");
  fclose(v);
}

double dInput (char* nome)
{
  double dIn;
  printf("Inserisci %s\n",nome);
  scanf("%lf",&dIn);
  return dIn;
}

//FUNZIONE dx/dt
double fx(double a, double b, double rho,double x0,double y0,double z0)
{
  double vx;
  vx=x0*(a*(1-x0)+0.5*(1-y0)+b*(1-z0));
  return vx;
}

//FUNZIONE dy/dt
double fy(double a, double b, double rho,double x0,double y0,double z0)
{
  double vy;
  vy=y0*(0.5*(x0-1.)+b*(y0-z0));
  return vy;
}

//FUNZIONE dz/dt
double fz(double a, double b, double rho,double x0,double y0,double z0)
{
  double vz;
  vz=z0*(rho*(1-x0)+0.1*(2-y0-z0));
  return vz;
}


double RK4(double a,double b,double rho, double x0, double y0, double z0, long int N,double dt)
{
  register int n; //variabile di conteggio
  double x[4],y[4],z[4],xn1,yn1,zn1,xn,yn,zn,vx1,vy1,vz1;
  double c,c0,xf,yf,zf;
  xn=x0;
  yn=y0;
  zn=z0;
  c0=log(x0)-x0+log(y0)-y0+z0;
  for(n=0;n<N;n++)
    {
      //Parametri per l'integrazione RK4
      x[0]=fx(a,b,rho,xn,yn,zn)*dt;
      y[0]=fy(a,b,rho,xn,yn,zn)*dt;
      z[0]=fz(a,b,rho,xn,yn,zn)*dt;
      x[1]=fx(a,b,rho,xn+0.5*x[0],yn+0.5*y[0],zn+0.5*z[0])*dt;
      y[1]=fy(a,b,rho,xn+0.5*x[0],yn+0.5*y[0],zn+0.5*z[0])*dt;
      z[1]=fz(a,b,rho,xn+0.5*x[0],yn+0.5*y[0],zn+0.5*z[0])*dt;
      x[2]=fx(a,b,rho,xn+0.5*x[1],yn+0.5*y[1],zn+0.5*z[1])*dt;
      y[2]=fy(a,b,rho,xn+0.5*x[1],yn+0.5*y[1],zn+0.5*z[1])*dt;
      z[2]=fz(a,b,rho,xn+0.5*x[1],yn+0.5*y[1],zn+0.5*z[1])*dt;
      x[3]=fx(a,b,rho,xn+x[2],yn+y[2],zn+z[2])*dt;
      y[3]=fy(a,b,rho,xn+x[2],yn+y[2],zn+z[2])*dt;
      z[3]=fz(a,b,rho,xn+x[2],yn+y[2],zn+z[2])*dt;
      //variabili coordinate aggiornate 
      xn1=xn+(x[0]+2.*x[1]+2.*x[2]+x[3])/6.;
      yn1=yn+(y[0]+2.*y[1]+2.*y[2]+y[3])/6.;
      zn1=zn+(z[0]+2.*z[1]+2.*z[2]+z[3])/6.;           
      //variabili velocità
      vx1=fx(a,b,rho,xn,yn,zn);
      vy1=fy(a,b,rho,xn,yn,zn);
      vz1=fz(a,b,rho,xn,yn,zn);
      //riassegnazione necessaria per eseguire il ciclo
      xn=xn1;
      yn=yn1;
      zn=zn1;
      //calcolo della costante del moto
      c=log(xn)-xn+log(yn)-yn+zn;
      return c;
    }
}

void banner()
{
  printf("%s\n%s\n%s\n%s\n",
     "#Questo programma integra il sistema di equazioni differenziali",
     "#La visualizzazione su schermo potrebbe impiegare qualche secondo a partire perchè per questioni di efficienza",
     "#ho deciso di stampare su stdout dopo la memorizzazione su stringa. Questo ha permesso di ridurre il tempo",
     "#complessivo di computazione");
}

void RK4b(double a,double b,double rho, double x0, double y0, double z0, long int N,double dt)
{
  FILE *u;
  FILE *ln;
  FILE* per;
  per=fopen("periodo.dat","w+");
  u=fopen("coordinate.dat","w+");
  ln=fopen("log.dat","w+");
  int n; //variabile di conteggio
  double x[4],y[4],z[4],xn1,yn1,zn1,xn,yn,zn,vx1,vy1,vz1;
  double c,c0,xf,yf,zf;
  xn=x0;
  yn=y0;
  zn=z0;
  char *pPeriodo=malloc(N*3);
  char *pStampa= malloc(N*80);
  char *p=pStampa;
  char *p1=pPeriodo;
  char *pLog=malloc(40*N);
  char *p2=pLog;
  memset(pStampa,0x00,N*80);
  memset(pPeriodo,0x00,N*3);
  memset(pLog,0x00,N*40);
  printf("#****************************************** inizio calcolo");
  for(n=0;n<N;n++)
    {
      //Parametri per l'integrazione RK4
      x[0]=fx(a,b,rho,xn,yn,zn)*dt;
      y[0]=fy(a,b,rho,xn,yn,zn)*dt;
      z[0]=fz(a,b,rho,xn,yn,zn)*dt;
      x[1]=fx(a,b,rho,xn+0.5*x[0],yn+0.5*y[0],zn+0.5*z[0])*dt;
      y[1]=fy(a,b,rho,xn+0.5*x[0],yn+0.5*y[0],zn+0.5*z[0])*dt;
      z[1]=fz(a,b,rho,xn+0.5*x[0],yn+0.5*y[0],zn+0.5*z[0])*dt;
      x[2]=fx(a,b,rho,xn+0.5*x[1],yn+0.5*y[1],zn+0.5*z[1])*dt;
      y[2]=fy(a,b,rho,xn+0.5*x[1],yn+0.5*y[1],zn+0.5*z[1])*dt;
      z[2]=fz(a,b,rho,xn+0.5*x[1],yn+0.5*y[1],zn+0.5*z[1])*dt;
      x[3]=fx(a,b,rho,xn+x[2],yn+y[2],zn+z[2])*dt;
      y[3]=fy(a,b,rho,xn+x[2],yn+y[2],zn+z[2])*dt;
      z[3]=fz(a,b,rho,xn+x[2],yn+y[2],zn+z[2])*dt;
      //variabili coordinate aggiornate 
      xn1=xn+(x[0]+2.*x[1]+2.*x[2]+x[3])/6.;
      yn1=yn+(y[0]+2.*y[1]+2.*y[2]+y[3])/6.;
      zn1=zn+(z[0]+2.*z[1]+2.*z[2]+z[3])/6.;     
      vx1=fx(a,b,rho,xn,yn,zn);
      vy1=fy(a,b,rho,xn,yn,zn);
      vz1=fz(a,b,rho,xn,yn,zn);
      xn=xn1;
      yn=yn1;
      zn=zn1;
      //TROVO IL PERIODO FISSANDO DELLE COORDINATE DI RIFERIMENTO E PONENDO LA CONDIZIONE DI RITORNO A TALI COORDINATE
      if(n==N/8)
    {
      xf=xn;
      yf=yn;
      zf=zn;
    }
      if(fabs(xf-xn)<=EPS && fabs(yf-yn)<=EPS && fabs(zf-zn)<=EPS)
    {
      sprintf(pPeriodo,"%lf\n",n*dt);
      p1+=strlen(p1);
    }
      //FINE PERIODO     
      sprintf(p,"%5.10lf\t%5.10lf\t%5.10lf\t%5.10lf\n",n*dt,xn,yn,zn);
      c=log(xn)-xn+log(yn)-yn;
      p+=strlen(p);
      sprintf(pLog,"%4.10lf\t%2.16lf\n",c,n*dt);
      p2+=strlen(p2);
    }
  printf("#fine calcolo ****************************************\n");
  fprintf(per,"%s",pPeriodo); //Stampa su file dei periodi 
  fprintf(u,"%s",pStampa); //Stampa su file delle coordinate in funzione del tempo
  fprintf(stdout,"%s",pStampa); //Stampa su stdout delle coordinate in funzione del tempo
  fprintf(ln,"%s",pLog);
  printf("#FINE STAMPA\n");
  free (pLog);
  free (pStampa);
  free (pPeriodo);
  fclose (u);
}

void start_time(void)
{
  tk.begin_clock=tk.save_clock=clock();
  tk.begin_time=tk.save_time=time(NULL);
}

double prn_time(void)
{
  char s1[MAXSTRING], s2[MAXSTRING];
  int field_width, n1, n2;
  double clocks_per_second=(double)CLOCKS_PER_SEC, user_time, real_time;

  user_time=(clock()-tk.save_clock)/ clocks_per_second;
  real_time=difftime(time(NULL),tk.save_time);
  tk.save_clock=clock();
  tk.save_time=time(NULL);
  
  //STAMPA DEI VALORI TROVATI
  n1=sprintf(s1,"#%1.f",user_time);
  n2=sprintf(s2,"#%1.f",real_time);
  field_width=(n1>n2) ? n1:n2;
  printf("%s%*.1f%s\n%s%*.1f%s\n\n",
     "#user time: ",field_width,user_time,"seconds",
     "#real time; ",field_width,real_time,"seconds");
  return user_time;
}