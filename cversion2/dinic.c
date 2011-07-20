#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct net * Net;
typedef struct ladoxy *ladoxy;
typedef struct nodo nodo;
typedef struct cola * cola;
typedef struct path * path;
typedef struct snodo snodo;
typedef struct output * output;
typedef struct bfselem bfselem;
typedef struct colabfs *colabfs; 

struct ladoxy{
  unsigned int x:14;
  unsigned int y:14;
  unsigned int C:31; // Capacidad
  unsigned int f:31; // Flujo
};

struct nodo{
  unsigned int level; //Para hacer networks auxiliares
  unsigned int a:14; // Ancestro
  unsigned int b:1; // Balance
  unsigned int n_vecinosforw:14; // Numero de vecinos forward
  unsigned int n_vecinosback:14; // Numero de vecinos backward
  unsigned int n_startforw:14; //para Dinic
  unsigned int n_startback:14; // para Dinic
  unsigned int *vecinosforw; // Indice de vecinos_forwward
  unsigned int *vecinosback; // Indice de vecinos_backward
};

struct net {
  ladoxy *ladosxy; // Array de lados con sus capacidades y flujo
  unsigned int *numeros; // numeros correspondientes a cada indice
  unsigned int n_lados;
  unsigned int n_nodos;
  unsigned short int *n_xmasy;
  nodo nodos[0]; // Array de nodos con sus repectivos datos
};

struct cola{
  unsigned int start; // La posición del elemento inicial
  unsigned int end; // La posición del elemento final start<end
  unsigned int size; // Indica el tamaño de tail end<size
  unsigned int * set; // Array de booleanos que indica cuales son los elementos en la cola
  unsigned int tail[0]; // Array de elementos
};

struct bfselem{
  unsigned int id:16;
};

struct colabfs{
  unsigned size;
  unsigned end;
  unsigned start;
  bfselem tail[0];
};

struct snodo{
  unsigned int balance:1;
  unsigned int nombre:15;
};

struct path{
  unsigned int flujo:31;
  unsigned int n_nodos:17;
  snodo nodos[0];
};
  
struct output{
  path *caminosNA; /*Agregado para la impresion de Dinic*/
  unsigned int n_caminosNA; /*Agregado para la impresion de Dinic*/
  unsigned int n_caminos;
  unsigned int flujo;
  cola corteminimal;
  path * caminos;
  Net net;
  unsigned int ttime;
};

#define VERBOSO      1
#define MEDIR_T      2
#define NUMERICO     4
#define INVALIDARGS  8
#define seta(y,net,x) net->nodos[y].a=x
#define setb(y,net,x) net->nodos[y].b=x
#define a(y,net) net->nodos[y].a
#define b(y,net) net->nodos[y].b 
#define rem_cola(Q) Q->start++
#define head_cola(Q) Q->tail[Q->start]
#define isnotempty(Q) (Q->end - Q->start)
#define in_t(Q) Q->tail[Q->end-1]==1
#define setflujo(r,S) S->flujo=r
#define setv(v,out) out->flujo=v
#define ISNUMBER(x) ((strspn(x, "0123456789")==strlen(x)) ? 1: 0)
#define rem_dinic(Q) Q->end--
#define noemptybfs(Q) !(Q->start==Q->end)
#define level(i,Q) Q->tail[i].level
#define bfsid(i,Q) Q->tail[i].id
#define bfsaid(i,Q) Q->tail[i].aid
#define bfsbal(i,Q) Q->tail[i].balance
#define bfsend(Q) Q->end
#define startbfs(Q) Q->start

void colabfs_destroy(colabfs cola);
void destroy_output(output out);
void cola_destroy(cola cola);
void destroycamdinic(output out);
unsigned int vecinobackwarddex(unsigned int x, Net net);
void reset_NA(Net net);
void makeNA(Net net);
void resetstartvec(Net net);
void add_colabfs(unsigned int i, colabfs *Qq, unsigned int level, Net net);
void addvecinoNA(unsigned int x, unsigned int y, Net net, unsigned int bal);
colabfs colabfs_new(Net net);
unsigned int headbfs(colabfs Q);
void addcaminoNA(unsigned int i, colabfs Q, Net net);
void addallvecinosbfs(unsigned int i, colabfs *Qq, Net net);
unsigned int vecinoforwarddex(unsigned x, Net net);
void borrarvec(unsigned x, Net net, unsigned int bal);
void print_caminosdinic(output out,int flags);
void addpathdinic(path S, output out);
unsigned int capacidad_corteminimal(output out);
void resetnet(output out);
int dinic(Net net, output*outp, int times, int flags);
void printcaminosek(output out, int num);
void printcaminoa(Net net, path camino);
void printcaminon(Net net, path camino);
void print_ncorteminimal(output out);
void print_acorteminimal(output out);
void addminimalcut(cola Q, output out);
void printout(output out, int flags, int time[], int runs);
Net start_net(int flags);
unsigned int minx(unsigned int x, unsigned int y);
void start_cola (cola Q,Net net);
void add_cola (unsigned int e,cola *Q);
int check_args(int argc, char ** argv, int *times, int *ntimes);
void add_nnode(unsigned int *n, Net network);
void add_nnodos(unsigned int *x, unsigned int *y, Net network);
void add_anode(unsigned int *n, Net net);
void add_anodos(unsigned int *x,unsigned int *y, Net net);
void addvecino(Net network, unsigned int x, unsigned int balance);
void add_nlados(unsigned int x,unsigned int y, unsigned int C, Net network);
Net startnet(int flags);
unsigned int in_vecinosforward_menos(unsigned int x,cola Q,unsigned int *i,Net net);
unsigned int in_vecinosbackward_menos(unsigned int x,cola Q,unsigned int *i,Net net);
unsigned int f(unsigned int x, unsigned int y, Net net );
unsigned int C(unsigned int x, unsigned int y, Net net );
void setf(unsigned int x, unsigned int y, Net net ,unsigned int f);
output createout(void);
path createpath(void);
void destroy_path(path);
void addsnode (unsigned int i, unsigned int b, path *S);
void addpath(path S, output out);
void ek(Net net, output* output);
int main(int argc, char ** argv);
cola cola_new (Net net);
void addnet(Net net, output out);
void destroy_net(Net net);

void colabfs_destroy(colabfs cola){
  free(cola);
}

void destroy_net(Net net){
  unsigned int i;
  if(net->ladosxy){
    for(i=0;i<net->n_nodos*2; i++){
      if(net->ladosxy[i])
	free(net->ladosxy[i]);
    }
    free(net->ladosxy);
  }

  if(net->numeros)
    free(net->numeros);
  
  if(net->n_xmasy)
    free(net->n_xmasy);

  for(i=0;i<net->n_nodos;i++){
    if(net->nodos[i].vecinosforw)
      free(net->nodos[i].vecinosforw);
    if(net->nodos[i].vecinosback)
      free(net->nodos[i].vecinosback);
  }

  free(net);
}  

void destroy_output(output out){
  unsigned int i;
  if(out->corteminimal)
    cola_destroy(out->corteminimal);
  if(out->caminos){
    for(i=0;i<out->n_caminos;i++)
      free(out->caminos[i]);
    free(out->caminos);
  }
  if(out->net)
    destroy_net(out->net);
  free (out);
}

unsigned int minx (unsigned int x, unsigned int y){
  return (x<y?x:y);
}

cola cola_new (Net net)
{
  cola Q;
  Q = calloc (1, sizeof(struct cola)+100*sizeof(int));
  Q->set = calloc ((net->n_nodos/32+1), sizeof(int));
  Q->size = 100;
  return Q;
}

void start_cola (cola Q,Net net)
{
  Q->start = 0;
  Q->end = 1;
  Q->tail[0] = 0;
  memset(Q->set, 0, (net->n_nodos/32+1) * sizeof(int));
  Q->set[0]=1;/*Porque s esta en el lugar 1*/
}

void add_cola (unsigned int e, cola *Qq)
{
  cola Q=*Qq;
  Q->set[e >> 5]|=1 << (e&31);
  Q->tail[Q->end]=e;
  Q->end++;
  if (Q->size==Q->end+1){
    *Qq=realloc(Q, sizeof(struct cola)+Q->size * 2 * sizeof(int));
    (*Qq)->size*=2;
  }
}

int check_args(int argc, char ** argv, int *times, int *ntimes){
  int tempi,flags=0;
  char tempc[20];
  
  /*flujo A B C D*/
  // A
  if (argc<6||argc>7) return INVALIDARGS;
  
  sscanf(argv[1], "%s", tempc);
  if (argc==6){
    if (!(strcmp(tempc,"-t"))){
      sscanf(argv[2], "%s", tempc);
      if (!ISNUMBER(tempc)) return INVALIDARGS;
      tempi=atoi(tempc);
      if(tempi<0) return INVALIDARGS;
      if(tempi>0){
	flags|=MEDIR_T;
	*ntimes=tempi;
      }
    }
    else return INVALIDARGS;
    
    sscanf(argv[3], "%s", tempc);
    if (!(strcmp(tempc,"-d"))){
      sscanf(argv[4], "%s", tempc);
      if (!ISNUMBER(tempc)) return INVALIDARGS;
      *times=atoi(tempc);
      if (*times<0) return INVALIDARGS;
    } else return INVALIDARGS;
    
    sscanf(argv[5], "%s", tempc);
    if (!(strcmp(tempc,"-n")))
      flags|=NUMERICO;
    else if ((strcmp(tempc,"-a")))
      return INVALIDARGS;
  }
  else {
    if (!(strcmp(tempc,"-v")))  flags|=VERBOSO;
    
    sscanf(argv[2], "%s", tempc);
    if (!(strcmp(tempc,"-t"))){
      sscanf(argv[3], "%s", tempc);
      if (!ISNUMBER(tempc)) return INVALIDARGS;
      tempi=atoi(tempc);
      if (tempi<0) return INVALIDARGS;
      if (tempi>0){
	flags|=MEDIR_T;
	*ntimes=tempi;
      }
    } else return INVALIDARGS;
    
    sscanf(argv[4], "%s", tempc);
    if (!(strcmp(tempc,"-d"))){
      sscanf(argv[5], "%s", tempc);
      if (!ISNUMBER(tempc)) return INVALIDARGS;
      *times=atoi(tempc);
      if (*times<0) return INVALIDARGS;
    } else return INVALIDARGS;
    
    sscanf(argv[6], "%s", tempc);
    if (!(strcmp(tempc,"-n")))
      flags|=NUMERICO;
    else if ((strcmp(tempc,"-a")))
      return INVALIDARGS;
  }
  return flags;
}

void add_nnode(unsigned int *n, Net net){
  unsigned int n_nodos=net->n_nodos;

  if(*n!=0&&*n!=1){
    net->nodos[n_nodos].level=(unsigned int)-1;
    net->nodos[n_nodos].b=0;
    net->nodos[n_nodos].a=0;
    net->nodos[n_nodos].n_vecinosforw=0; // Numero de vecinos forward
    net->nodos[n_nodos].n_vecinosback=0; //Numero de vecinos backward
    net->nodos[n_nodos].vecinosforw=NULL; //indice de vecinos forward
    net->nodos[n_nodos].vecinosback=NULL; //indice de vecinos backward
    net->nodos[n_nodos].n_startforw=0;
    net->nodos[n_nodos].n_startback=0;
    net->numeros[n_nodos]=*n;
    net->n_nodos++;
    *n=n_nodos;
  } else if(*n!=1){
    net->nodos[0].level=0;
    net->nodos[0].b=0;
    net->nodos[0].a=0;
    net->nodos[0].n_vecinosforw=0; // Numero de vecinos forward
    net->nodos[0].n_vecinosback=0; //Numero de vecinos backward
    net->nodos[0].vecinosforw=NULL; //indice de vecinos forward
    net->nodos[0].vecinosback=NULL; //indice de vecinos backward
    net->nodos[0].n_startforw=0;
    net->nodos[0].n_startback=0;
    net->numeros[0]=0;
    *n=0;
  } else {
    net->nodos[1].level=(unsigned int)-1;
    net->nodos[1].b=0;
    net->nodos[1].a=0;
    net->nodos[1].n_vecinosforw=0; // Numero de vecinos forward
    net->nodos[1].n_vecinosback=0; //Numero de vecinos backward
    net->nodos[1].vecinosforw=NULL; //indice de vecinos forward
    net->nodos[1].vecinosback=NULL; //indice de vecinos backward
    net->nodos[1].n_startforw=0;
    net->nodos[1].n_startback=0;
    net->numeros[1]=1;
    *n=1;
  }
}

void add_nnodos(unsigned int *x,unsigned int *y, Net net){
  unsigned int i, isinx=0, isiny=0;
  
  for(i=0; i<net->n_nodos;i++){
    if(!isinx && net->numeros[i]==*x){
      isinx=1;
      *x=i;
    } else if(!isiny && net->numeros[i]==*y){
      isiny=1;
      *y=i;
    }
  }
  if (!isinx) add_nnode(x, net);
  if (!isiny) add_nnode(y, net);
}


void add_anode(unsigned int *n, Net net){
  unsigned int n_nodos=net->n_nodos;


  if(*n!='s'&&*n!='t'){
    net->nodos[n_nodos].level=(unsigned int)-1;
    net->nodos[n_nodos].b=0;
    net->nodos[n_nodos].a=0;
    net->nodos[n_nodos].n_vecinosforw=0; // Numero de vecinos forward
    net->nodos[n_nodos].n_vecinosback=0; //Numero de vecinos backward
    net->nodos[n_nodos].vecinosforw=NULL; //indice de vecinos+vecinos_backward
    net->nodos[n_nodos].vecinosback=NULL; //indice de vecinos+vecinos_backward
    net->nodos[n_nodos].n_startforw=0;
    net->nodos[n_nodos].n_startback=0;
    net->numeros[n_nodos]=*n;
    net->n_nodos++;
    *n=n_nodos;
  } else if(*n!='t'){
    net->nodos[0].level=0;
    net->nodos[0].b=0;
    net->nodos[0].a=0;
    net->nodos[0].n_vecinosforw=0; // Numero de vecinos forward
    net->nodos[0].n_vecinosback=0; //Numero de vecinos backward
    net->nodos[0].vecinosforw=NULL; //indice de vecinos+vecinos_backward
    net->nodos[0].vecinosback=NULL; //indice de vecinos+vecinos_backward
    net->nodos[0].n_startforw=0;
    net->nodos[0].n_startback=0;
    net->numeros[0]=(unsigned int)'s';
    *n=0;
  } else {
    net->nodos[1].level=(unsigned int)-1;
    net->nodos[1].b=0;
    net->nodos[1].a=0;
    net->nodos[1].n_vecinosforw=0; // Numero de vecinos forward
    net->nodos[1].n_vecinosback=0; //Numero de vecinos backward
    net->nodos[1].vecinosforw=NULL; //indice de vecinos+vecinos_backward
    net->nodos[1].vecinosback=NULL; //indice de vecinos+vecinos_backward
    net->nodos[1].n_startforw=0;
    net->nodos[1].n_startback=0;
    net->numeros[1]=(unsigned int)'t';
    *n=1;
  }
}

void add_anodos(unsigned int *x,unsigned int *y, Net net){
  unsigned int i, isinx=0, isiny=0;
  
  for(i=0; i<net->n_nodos;i++){
    if(!isinx && net->numeros[i]==*x){
      isinx=1;
      *x=i;
    } else if(!isiny && net->numeros[i]==*y){
      isiny=1;
      *y=i;
    }
  }
  if (!isinx) add_nnode(x, net);
  if (!isiny) add_nnode(y, net);
}


void addvecino(Net net, unsigned int x, unsigned int y){
net->nodos[x].n_vecinosforw++;
  net->nodos[y].n_vecinosback++;
  net->nodos[x].vecinosforw=realloc(net->nodos[x].vecinosforw,
				net->nodos[x].n_vecinosforw*sizeof (unsigned int));
  net->nodos[y].vecinosback=realloc(net->nodos[y].vecinosback,
				net->nodos[y].n_vecinosback*sizeof (unsigned int));
  net->nodos[x].vecinosforw[net->nodos[x].n_vecinosforw-1]=y;
  net->nodos[y].vecinosback[net->nodos[y].n_vecinosback-1]=x;
}

void add_nlados(unsigned int x,unsigned int y, unsigned int C, Net network){
  network->n_xmasy[x+y]++;
  network->ladosxy[x+y]=realloc(
				network->ladosxy[x+y],
				sizeof(struct ladoxy)
				* network->n_xmasy[x+y]);
  network->ladosxy[x+y][network->n_xmasy[x+y]-1].x=x;
  network->ladosxy[x+y][network->n_xmasy[x+y]-1].y=y;
  network->ladosxy[x+y][network->n_xmasy[x+y]-1].C=C;
  network->ladosxy[x+y][network->n_xmasy[x+y]-1].f=0;
  network->n_lados++;
  addvecino(network,x,y);
}


void printout(output out, int flags, int time[], int runs){
  int cap;
  if((flags&VERBOSO)&&!(flags&MEDIR_T))
    printcaminosek(out,flags&NUMERICO);

  printf("Valor del flujo: %i\n",out->flujo);
  printf("Corte Minimal: S={");
  if (flags&NUMERICO)
    print_ncorteminimal(out);
  else
    print_acorteminimal(out);

  cap=capacidad_corteminimal(out);

  printf("Capacidad del Corte Minimal: %u\n", cap);

  if(flags&MEDIR_T){
    printf("TIEMPO LEYENDO INPUT: %u segundos.\n", time[0]);
    printf("TIEMPO TOTAL para %i corridas: %u segundos.\n", runs, time[1]);
  }
}

unsigned int capacidad_corteminimal(output out){
  unsigned int cap=0, i, j, k, y;
  for(i=0; i<out->corteminimal->end;i++){
    j=out->corteminimal->tail[i];
    for(k=0; (y=in_vecinosforward_menos(j,out->corteminimal,&k,out->net)); k++){
      cap+=f(j,y,out->net);
    }
  }
  return cap;
}

void printcaminosek(output out, int num){
  unsigned int i;
  
  if(num){
    for(i=0; i<out->n_caminos; i++)
      printcaminon(out->net,out->caminos[i]);
  }
  else{
    for(i=0; i<out->n_caminos; i++)
      printcaminoa(out->net,out->caminos[i]);
  }
}

void printcaminoa(Net net, path camino){
  int i;
  printf("s");
  for(i=camino->n_nodos-2;i>0;i--){
    if(camino->nodos[i].balance)
      printf("%c",(char)net->numeros[camino->nodos[i].nombre]);
    else
      printf("<%c",(char)net->numeros[camino->nodos[i].nombre]);
  }
  printf("t:%u\n", camino->flujo);
}

void printcaminon(Net net, path camino){
  int i;

  printf("s.");
  for(i=camino->n_nodos-2;i>0;i--){
    if(camino->nodos[i].balance){
      printf("%u.",net->numeros[camino->nodos[i].nombre]);
    }
    else
      printf("<%u.",net->numeros[camino->nodos[i].nombre]);
  }
  printf("t:%u\n", camino->flujo);
}

void print_acorteminimal(output out){
  unsigned int i;
  for(i=out->corteminimal->end-1; i>=1; i--)
    printf("%c,",(char)out->net->numeros[out->corteminimal->tail[i]]);
  if(out->corteminimal->end>0)
    printf("%c",(char)out->net->numeros[out->corteminimal->tail[0]]);
  printf("}\n");
}

void print_ncorteminimal(output out){
  unsigned int i;
  for(i=out->corteminimal->end-1; i>=1; i--)
    printf("%u,",out->net->numeros[out->corteminimal->tail[i]]);
  if(out->corteminimal->end>0)
    printf("%u",out->net->numeros[out->corteminimal->tail[0]]);
  printf("}\n");
}


void addnet(Net net, output out){
  out->net=net;
}

Net start_net(int flags){
  Net net=NULL;
  int scanr=0;
  unsigned int ladosr=1000,
    ladost = 0,
    nodosr = 1000,
    x=0,y=0,cap=0;
  
  //freopen ("input", "r", stdin);
  
  net = calloc(1,sizeof (struct net) + nodosr * sizeof (nodo));
  net->n_xmasy = calloc(ladosr, sizeof (unsigned short int));
  net->numeros = malloc (nodosr * sizeof (unsigned int));
  net->ladosxy = calloc (ladosr, sizeof (void*));
  net->n_nodos=2;
  net->n_lados=0;
  
  if (flags & NUMERICO) {
    net->numeros[0]=0;
    net->numeros[1]=1;
    while ((scanr=scanf ("%u %u %u\n", &x, &y, &cap)) != EOF && scanr==3 ){
      add_nnodos(&x,&y,net);
      add_nlados(x,y, cap, net);
      if (net->n_nodos+1>=nodosr){
	nodosr*=2;
	net = realloc(net,sizeof (struct net) + nodosr * sizeof (nodo));
	net->numeros=realloc(net->numeros,nodosr * sizeof (unsigned int));
      }
      if (net->n_nodos*2+2>=ladosr){
	ladost = ladosr;
	ladosr=(ladosr)*2;
	net->ladosxy=realloc(net->ladosxy,ladosr * sizeof (void*));
	net->n_xmasy=realloc(net->n_xmasy, ladosr * sizeof (unsigned short int));
	memset(&net->ladosxy[ladost], 0, (ladosr-ladost) * sizeof (void*));
	memset(&net->n_xmasy[ladost], 0, (ladosr-ladost) * sizeof (unsigned short int));
      }
    }
  } else {
    net->numeros[0]=(unsigned int)'s';
    net->numeros[1]=(unsigned int)'t';
    while ((scanr=scanf ("%c%c %u\n", (char*)&x, (char*)&y, &cap)) != EOF && scanr==3){
      add_anodos(&x,&y,net);
      add_nlados(x,y, cap, net);
      if (net->n_nodos+1>=nodosr){
	nodosr*=2;
	net = realloc(net,sizeof (struct net) + nodosr * sizeof (nodo));
	net->numeros=realloc(net->numeros,nodosr * sizeof (unsigned int));
      }
      if (net->n_nodos*2+2>=ladosr){
	ladost = ladosr;
	ladosr=(ladosr+2)*2;
	net->ladosxy=realloc(net->ladosxy, ladosr * sizeof (void*));
	net->n_xmasy = realloc(net->n_xmasy, ladosr * sizeof (unsigned short int));
	memset(&net->ladosxy[ladost], 0, (ladosr-ladost) * sizeof (void*));
	memset(&net->n_xmasy[ladost], 0, (ladosr-ladost) * sizeof (unsigned short int));
      }
    }
  }

  if(scanr<3&&scanr>=0){
    destroy_net(net);
    net=NULL;
  }
  
  return net;
}

unsigned int in_vecinosforward_menos(unsigned int x,cola Q,unsigned int *i,Net net){
  unsigned int j=*i, nvec=net->nodos[x].n_vecinosforw, vec;
  
  for(; j<nvec;j++){
    vec=net->nodos[x].vecinosforw[j];
    if(!((1 << (vec&31))&Q->set[vec >> 5])){
      *i=j;
      return vec; 
    }
  }
  
  return 0;
}

unsigned int in_vecinosbackward_menos(unsigned int x,cola Q,unsigned int *i,Net net){
  unsigned int j=*i, nvec=net->nodos[x].n_vecinosback, vec;

  for(; j<nvec;j++){
    vec=net->nodos[x].vecinosback[j];
    if(!((1 << (vec&31))&Q->set[vec >> 5])){
      *i=j;
      return vec; 
    }
  }
  return 0;
}

unsigned int f(unsigned int x, unsigned int y, Net net ){
  unsigned int i, maxxy=net->n_xmasy[x+y];
  
  for(i=0; i<maxxy; i++){
    if(net->ladosxy[x+y][i].x==x){
      return net->ladosxy[x+y][i].f;
    }
    if (net->ladosxy[x+y][i].x==y){
      return net->ladosxy[x+y][i].f;
    }
  }

  return 0;
}

unsigned int C(unsigned int x, unsigned int y, Net net ){
  unsigned int i, maxxy=net->n_xmasy[x+y];
  
  for(i=0; i<maxxy; i++){
    if(net->ladosxy[x+y][i].x==x){
      return net->ladosxy[x+y][i].C;
    }
    if (net->ladosxy[x+y][i].x==y){
      return net->ladosxy[x+y][i].C;
    }
  }
  return 0;
}

void setf(unsigned int x, unsigned int y, Net net ,unsigned int f){
  unsigned int i, maxxy=net->n_xmasy[x+y];
  
  for(i=0; i<maxxy; i++){
    if(net->ladosxy[x+y][i].x==x){
      net->ladosxy[x+y][i].f=f;
      break;
    }
    if (net->ladosxy[x+y][i].x==y){
      net->ladosxy[x+y][i].f=f;
      break;
    }
  }
}

output createout(){
  output out;

  out=calloc(1,sizeof(struct output));

  return out;
}

path createpath(){
  path camino;
  camino=malloc(sizeof(struct path)+sizeof(struct snodo));
  camino->n_nodos=0;
  return camino;
}

void destroy_path(path Path){
  free (Path);
}


void addsnode (unsigned int i, unsigned int b, path *Sp){
  path S=*Sp;
  S->n_nodos++;
  *Sp=realloc(S,sizeof(struct path)+(S->n_nodos) * sizeof (struct snodo));
  S=*Sp;
  S->nodos[S->n_nodos-1].balance=b;
  S->nodos[S->n_nodos-1].nombre=i;
}

void addpath(path S, output out){
  out->n_caminos++;
  out->caminos=realloc(out->caminos,
		       sizeof(path) * out->n_caminos); //path es struct path*
  out->caminos[out->n_caminos-1]=S;
}

void addminimalcut(cola Q, output out){
  out->corteminimal=Q;
}

void ek(Net net, output* outputp){
  output out=NULL;
  path S=NULL;
  unsigned int x=0, y=0, i=0, r=0, end=0, v=0;
  cola Q=cola_new(net);

  out=(*outputp);
  if(!out)out=createout();

  v=out->flujo;
  out->net=net;

  do {
    start_cola(Q,net);
    S=createpath();
    end = 0;
    
    while (isnotempty(Q)) {
      x= head_cola(Q);

      for(i=0; (y=in_vecinosforward_menos(x,Q,&i,net));i++){
	if (f(x,y,net) < C(x,y,net)){
	  add_cola(y,&Q);
	  seta(y,net,x); // “Ancestro” de y
	  setb(y,net,1); // “Forward”
	  if (y==1){
	    end=1;
	    break;
	  }
	}
      }
      if (end) break;
      
      for (i=0; (y=in_vecinosbackward_menos(x,Q,&i,net));i++){
	if (f(y,x,net)){
	  add_cola(y,&Q);
	  seta(y,net,x);  // “Ancestro” de y
	  setb(y,net,0); // “Backward”
	}
      }
      rem_cola(Q); //cambia el indice inicial en Q
    }
    
    if (in_t(Q)){
      y=1,r=-1; // -1 porque es unsigned asi que -1 es el maximo valor
      while(y != 0){
	x=a(y,net);
	if (b(y,net)){
	  r=minx(C(x,y,net)-f(x,y,net),r);
	}
	else 
	  r=minx(f(y,x,net),r);
	y=x;
      }
      
      y=1, v+=r;
      
      while (y != 0){
	x=a(y,net);
	addsnode(y,b(y,net),&S);
	if (b(y,net))
	  setf(x,y,net,f(x,y,net)+r);
	else 
	  setf(y,x,net,f(y,x,net)-r);
	y=x;
      }
      addsnode(y,b(y,net),&S);
      setflujo(r,S);
      addpath(S,out);
    }
    
  } while (in_t(Q));

  destroy_path(S);
  setv(v,out);
  addminimalcut(Q,out);
  addnet(net,out);

  *outputp=out;

}

unsigned int vecinoforwarddex(unsigned int x, Net net){
  unsigned int i, vec=0;
  i=net->nodos[x].n_startforw;
  while(i<net->nodos[x].n_vecinosforw){
    vec=net->nodos[x].vecinosforw[i];
    if (net->nodos[x].level < net->nodos[vec].level && f(x,vec,net)<C(x,vec,net))
      break;
    else {
      i=++net->nodos[x].n_startforw;
      vec=0;
    }
  }
  return vec;
}

unsigned int vecinobackwarddex(unsigned int x, Net net){
  unsigned int i, vec=0;
  i=net->nodos[x].n_startback;
  while(i<net->nodos[x].n_vecinosback){
    vec=net->nodos[x].vecinosback[i];
    if (net->nodos[x].level < net->nodos[vec].level && f(vec,x,net)>0)
      break;
    else {
      i=++net->nodos[x].n_startback;
      vec=0;
    }
  }
  return vec;
}

void borrarvec(unsigned int x, Net net, unsigned int bal){
  if(bal)
    net->nodos[x].n_startforw++;
  else
    net->nodos[x].n_startback++;
}

void print_caminosdinic(output out,int flags){
  unsigned int i;
  

  if(flags&NUMERICO){
    for(i=0; i<out->n_caminosNA; i++){
      printcaminon(out->net,out->caminosNA[i]);
    }
  }
  else{
    for(i=0; i<out->n_caminosNA; i++)
      printcaminoa(out->net,out->caminosNA[i]);
  }
}

void addpathdinic(path S, output out){
  out->n_caminosNA++;
  out->caminosNA=realloc(out->caminosNA, sizeof(path) * out->n_caminosNA); //path es struct path*
  out->caminosNA[out->n_caminosNA-1]=S;
}

colabfs colabfs_new(Net net){
  colabfs result;
  result=malloc(sizeof(struct colabfs) + net->n_nodos*sizeof(struct bfselem));
  result->start=0;
  result->end=1;
  result->size=net->n_nodos;
  result->tail[0].id=0;
  net->nodos[0].level=0;

  return result;
}

void add_colabfs(unsigned int i, colabfs *Qq, unsigned int level, Net net){
  colabfs Q=*Qq;

  if (net->nodos[i].level==(unsigned int)-1){
    Q->tail[Q->end].id=i;
    Q->end++;
    net->nodos[i].level=level;
    //    printf("agrego a %i a la cola. level=%u,\n", net->numeros[i], level);
  }
}

void addallvecinosbfs(unsigned int i, colabfs *Qq, Net net){
  colabfs Q=*Qq;
  unsigned int j, k, in=bfsid(i,Q);
  
  for (j=0;j<net->nodos[in].n_vecinosforw;j++){
    Q=*Qq;
    k=net->nodos[in].vecinosforw[j];
    if(f(in,k,net)<C(in,k,net))
      add_colabfs(k,Qq,net->nodos[in].level+1,net);
  }

  for (j=0;j<net->nodos[in].n_vecinosback;j++){
    Q=*Qq;
    k=net->nodos[in].vecinosback[j];
    if(f(k,in,net)>0&&k!=in)
      add_colabfs(k,Qq,net->nodos[in].level+1,net);
   }
}

void resetstartvec(Net net){
  unsigned int i;
 
  for(i=0;i<net->n_nodos;i++){
    net->nodos[i].n_startforw=0;
    net->nodos[i].n_startback=0;
  }
}

void makeNA(Net net){
  unsigned int i;
  colabfs Q=colabfs_new(net);

  for(i=startbfs(Q);noemptybfs(Q)&&net->nodos[bfsid(i,Q)].level<net->nodos[1].level;i=++startbfs(Q))
    addallvecinosbfs(i,&Q,net);
  
  resetstartvec(net);/*Resetea n_startforw y n_startback*/
  
  colabfs_destroy(Q);
}

void reset_NA(Net net){
  unsigned int i;

  for(i=1; i<net->n_nodos; i++)
    net->nodos[i].level=(unsigned int)-1;
}

void destroycamdinic(output out){
  unsigned int i;
  for(i=0; i<out->n_caminosNA;i++)
    free(out->caminosNA[i]);
  free(out->caminosNA);  
  out->caminosNA=NULL;
  out->n_caminosNA=0;
}

int dinic(Net net, output*outp, int times, int flags){
  unsigned int v=0, x=0, y=0, r=0, end=0;
  int i=0, stopflag=0;
  output out=*outp;
  path S=NULL;
  
  if(!out)out=createout();

  out->net=net;

  for(i=0;i<times && !end;i++){
    v=0;
    stopflag=0; 
    reset_NA(net);
    makeNA(net);
    end = net->nodos[1].level == (unsigned int)-1;
    while(!stopflag&&!end){
      S=createpath();
      x=0;
      while((x!=1)&&!stopflag){
	if ((y=vecinoforwarddex(x,net))){/*Avanzar en vecinos forward*/
	  setb(y,net,1);
	  seta(y,net,x);
	  x=y;
	}
	else if ((y=vecinobackwarddex(x,net))){/*Avanzar en vecinos backward*/
	  setb(y,net,0);
	  seta(y,net,x);
	  x=y;
	}
	else if (x){//x!=s /*Retroceder*/
	  y=a(x,net);

	  borrarvec(y,net,b(x,net));
	  x=y;
	}
	else stopflag=1;
      }

      if (x==1) {/*Aumentar*/
    	y=1,r=-1; // -1 porque es unsigned asi que -1 es el maximo valor
	while(y != 0){
	  x=a(y,net);
	  if(b(y,net)){
	    r=minx(C(x,y,net)-f(x,y,net),r);
	  }
	  else{
	    r=minx(f(y,x,net),r);
	  }
	  y=x;
	}
	
	y=1, v+=r;
	

	while (y != 0){
	  x=a(y,net);
	  addsnode(y,b(y,net),&S);
	  if (b(y,net)){
	    setf(x,y,net,f(x,y,net)+r);
	    if(f(x,y,net)==C(x,y,net)){
	      borrarvec(x,net,1);
	    }
	  }
	  else {
	    setf(y,x,net,f(y,x,net)-r);
	    if(f(y,x,net)==0){
	      borrarvec(x,net,0);
	    }
	  }
	  y=x;
	}
	addsnode(y,1,&S);
	setflujo(r,S);
	addpathdinic(S,out);
      }
    }
  
    free(S);
    out->flujo+=v;
    

    if((flags&VERBOSO) ){
      if (i>1)
      printf("CAMINOS DEL NETWORK AUXILIAR %u\n", i+1);
      else if (i==1)
	printf("CAMINOS DEL SEGUNDO NETWORK AUXILIAR\n");
      else 
	printf("CAMINOS DEL PRIMER NETWORK AUXILIAR\n");
      
      if(!end) print_caminosdinic(out,flags);
      
      if (i>1)
	printf("FIN NETWORK AUXILIAR %u\n", i+1);
      else if (i==1)
	printf("FIN DEL SEGUNDO NETWORK AUXILIAR\n");
      else
	printf("FIN DEL PRIMER NETWORK AUXILIAR\n");
    }

    reset_NA(net);
    if(!end){
      makeNA(net);
      end=net->nodos[1].level==(unsigned int)-1;
      reset_NA(net);
    }

    destroycamdinic(out);    
  }
  


  *outp=out;
  
  return end;
}

void cola_destroy(cola cola){
  free(cola->set);
  free(cola);
}

void resetnet(output out){
  
  unsigned int x=0, i=0;
  for(x=0;x<out->net->n_lados*2;x++){
    if(out->net->ladosxy[x]!=NULL){
      for (i=0;i<out->net->n_xmasy[x];i++){
	out->net->ladosxy[x][i].f=0;
      }
    }
  }
  
  cola_destroy(out->corteminimal);
  out->corteminimal=NULL;
}

int main(int argc, char ** argv){
  Net network=NULL;
  int flags=0, times=0, end=0, i=0, runs=0;
  int t[2];
  time_t t1, t2, t3;

  output out=NULL;

  
  flags=check_args(argc,argv,&times, &runs);

  if (flags==INVALIDARGS){
    printf("invalid arguments\n");
    return -1;
  }

  time(&t1);

  network = start_net(flags); /*aca leo de std_in y hago el network*/

  time(&t2);

  if (network==NULL){
    printf("invalid input\n");
    return -1;
  }

  if(flags&MEDIR_T){
    for(i=runs;i>0;i--){
      if(out)out->flujo=0;
      if (times){
	end=dinic(network,&out,times,0);
	
	ek(network,&out);
      }
      else
	ek(network,&out);

      if(i!=1) resetnet(out);
    }
  } else {
    if (times){
      end=dinic(network,&out,times,flags);
      if (!end) printf("FIN DINIC, CAMBIANDO A EDMONDS-KARP\n");
      ek(network,&out);
    }
    else
	ek(network,&out);
    
  }
  time(&t3);

  t[0] = (int) (t2-t1);
  t[1] = (int) (t3-t2);
  printout(out,flags, t, runs);
  destroy_output(out);


  return 0;
}
