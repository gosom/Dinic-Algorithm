#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

typedef struct net * Net;
typedef struct edge *edge;
typedef struct node node;
typedef struct queue * queue;
typedef struct path * path;
typedef struct snode snode;
typedef struct output * output;
typedef struct bfs_elem bfs_elem;
typedef struct queue_bfs *queue_bfs; 

typedef unsigned int uint;
typedef unsigned short int usint;

struct edge{
  uint x:14;
  uint y:14;
  uint C:31; // Capacidad
  uint f:31; // Flow
};

struct node{
  uint level; //Para hacer networks auxiliares
  uint a:14; // Ancestro
  uint b:1; // Balance
  uint n_neighbs_forw:14; // Numero de vecinos forward
  uint n_neighbs_back:14; // Numero de vecinos backward
  uint n_start_forw:14; //para Dinic
  uint n_start_back:14; // para Dinic
  uint *neighbs_forw; // Indice de vecinos_forwward
  uint *neighbs_back; // Indice de vecinos_backward
};

struct net {
  uint *ids; // ids correspondientes a cada indice
  uint n_nodes;
  edge *edges; // Array of edges
  usint *n_xplusy;
  uint n_edges;
  node nodes[0]; // Array of nodes
};

struct queue{
  uint start; // La posición del elemento inicial
  uint end; // La posición del elemento final start<end
  uint size; // Indica el tamaño de tail end<size
  uint * set; // Array de booleanos que indica cuales son los elementos en la cola
  uint tail[0]; // Array de elementos
};

struct bfs_elem{
  uint id:16;
};

struct queue_bfs{
  unsigned size;
  unsigned end;
  unsigned start;
  bfs_elem tail[0];
};

struct snode{
  uint balance:1;
  uint name:15;
};

struct path{
  uint flow:31;
  uint n_nodes:17;
  snode nodes[0];
};
  
struct output{
  path *pathsNA; /*Agregado para la impresion de Dinic*/
  uint n_pathsNA; /*Agregado para la impresion de Dinic*/
  uint n_paths;
  uint flow;
  queue cutminimal;
  path * paths;
  Net net;
  uint ttime;
};

#define VERBOSO      1
#define MEDIR_T      2
#define NUMERICO     4
#define INVALIDARGS  8
#define set_ancestor(net,y,x) net->nodes[y].a=x  /* usada */ 
#define set_balance(net,y,x) net->nodes[y].b=x  /* usada */
#define ancestor(net,y) net->nodes[y].a  /* usada */
#define balance(net,y) net->nodes[y].b   /* usada */
#define path_set_flow(s,r) s->flow=r  /* usada */
#define ISNUMBER(x) ((strspn(x, "0123456789")==strlen(x)) ? 1: 0) /*usada*/
#define qbfs_not_empty(q) !(q->start==q->end) /* usada */
#define qbfs_id(q,i) q->tail[i].id /* usada */
#define qbfs_start(q) q->start /* usada */


Net net_new(); 

void net_destroy(Net net);

uint net_neighb_forw(Net net, uint x); 
uint net_neighb_back(Net net, uint x);

void net_reset_start(Net net);


void net_aux_new(Net net); 
void net_aux_reset(Net net); 

void net_del_neighb(Net net, uint x, uint bal);


void queue_bfs_destroy(queue_bfs q);/* usada ninguna */
void out_path_destroy(output out); /* usada ninguna */

void queue_bfs_add(Net net, queue_bfs *Qq, uint i, uint level); /* usada ninguna */
queue_bfs queue_bfs_new(Net net);/* usada ninguna */
void queue_bfs_add_neighbs(queue_bfs *Qq, Net net, uint i);/* usada completa */


void print_pathsdinic(output out,int flags); /* usada completa */
void addpathdinic(path S, output out); /* usada ninguna */

uint out_mincut_capacity(output out); /*usada completa*/
int dinic(Net net, output*outp, int times, int flags); /* usada completa */

void printpathsek(output out, int num); /* usada completa */
void printpathn(Net net, path path); /*usada ninguna */
void print_ncutminimal(output out);

void out_add_mincut(output out, queue q);

void printout(output out, int flags, int time[], int runs); /* usada completa */

uint minx(uint x, uint y);  /* usada completa */


int check_args(int argc, char ** argv, int *times, int *ntimes); /* usada completa */
void net_add_node(Net network, uint *n); /*usada*/
void net_add_nodes(Net network, uint *x, uint *y);/*usada*/
void net_add_neighbs(Net network, uint x, uint balance);/*usada*/
void add_nedges(uint x,uint y, uint C, Net network);/*usada*/
uint in_neighbs_forward_menos(uint x,queue Q,uint *i,Net net); /* usada completa */

uint f(uint x, uint y, Net net ); /* usada ninguna */
uint C(uint x, uint y, Net net ); /* usada ninguna */
void setf(uint x, uint y, Net net ,uint f); /* usada ninguna*/
output createout(void); /* usada ninguna */
path path_new(void); /* usada ninguna */
void path_destroy(path); /* usada ninguna */
void path_add_node (path *S, uint i, uint b); /* usada ninguna */

int main(int argc, char ** argv);


void queue_bfs_destroy(queue_bfs queue){
  free(queue);
}

void net_destroy(Net net){
  uint i;
  if(net->edges){
    for(i=0;i<net->n_nodes*2; i++){
      if(net->edges[i])
	free(net->edges[i]);
    }
    free(net->edges);
  }

  if(net->ids)
    free(net->ids);
  
  if(net->n_xplusy)
    free(net->n_xplusy);

  for(i=0;i<net->n_nodes;i++){
    if(net->nodes[i].neighbs_forw)
      free(net->nodes[i].neighbs_forw);
    if(net->nodes[i].neighbs_back)
      free(net->nodes[i].neighbs_back);
  }

  free(net);
}  

uint minx (uint x, uint y){
  return (x<y?x:y);
}

int check_args(int argc, char ** argv, int *times, int *ntimes){
  int tempi,flags=0;
  char tempc[20];
  
  /*flow A B C D*/
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

#define SOURCE_ID 0
#define TARGET_ID 1

void net_add_node(Net net, uint *n){
  uint n_nodes=net->n_nodes, pos=0, 
    s=SOURCE_ID, t=TARGET_ID;

  /* Custom settings for each type of node */

  if(*n != s && *n != t){
    pos = n_nodes;
    net->nodes[pos].level = (uint)-1;
    net->ids[pos] = *n;
    net->n_nodes++;
  } else if(*n != t){
    pos = 0;
    net->nodes[pos].level = 0;
    net->ids[pos] = s;
  } else {
    pos = 1;
    net->nodes[pos].level = (uint)-1;
    net->ids[pos] = t;
  }

  /* Common settings */

  *n = pos;

  /* All other values are zero */
}

void net_add_nodes(Net net, uint *x,uint *y){
  uint i;
  bool x_in_net = false,
    y_in_net = false;

  for(i = 0; i < net->n_nodes; i++){
    if(!x_in_net && net->ids[i] == *x){
      x_in_net = true;
      *x = i;
    } else if(!y_in_net && net->ids[i] == *y){
      y_in_net = true;
      *y = i;
    }
  }

  if (!x_in_net) net_add_node(net, x);
  if (!y_in_net) net_add_node(net, y);
}


void net_add_neighbs(Net net, uint x, uint y){
  uint n_neighbs_forw = ++net->nodes[x].n_neighbs_forw;
  uint n_neighbs_back = ++net->nodes[y].n_neighbs_back;

  net->nodes[x].neighbs_forw = 
    realloc(net->nodes[x].neighbs_forw,
	    n_neighbs_forw * sizeof(uint));

  net->nodes[y].neighbs_back = 
    realloc(net->nodes[y].neighbs_back,
	    n_neighbs_back * sizeof(uint));

  net->nodes[x].neighbs_forw[n_neighbs_forw-1] = y;
  net->nodes[y].neighbs_back[n_neighbs_back-1] = x;
}

void net_add_edge(Net network, uint x,uint y, uint C){
  uint pos = network->n_xplusy[x+y];
  uint size = ++network->n_xplusy[x+y];

  network->edges[x+y] = realloc(network->edges[x+y],
				sizeof(struct edge)
				* size);
  network->edges[x+y][pos].x = x;
  network->edges[x+y][pos].y = y;
  network->edges[x+y][pos].C = C;
  network->edges[x+y][pos].f = 0;
  network->n_edges++;
  net_add_neighbs(network, x, y);
}


void printout(output out, int flags, int time[], int runs){
  int cap;
  if((flags&VERBOSO)&&!(flags&MEDIR_T))
    printpathsek(out,flags&NUMERICO);

  printf("Valor del flow: %i\n",out->flow);
  /*
  printf("Cut Minimal: S={");

  if (flags&NUMERICO)
    print_ncutminimal(out);
  else
    print_acutminimal(out);


  cap=out_mincut_capacity(out);

  printf("Capacidad del Cut Minimal: %u\n", cap);

  if(flags&MEDIR_T){
    printf("TIEMPO LEYENDO INPUT: %u segundos.\n", time[0]);
    printf("TIEMPO TOTAL para %i corridas: %u segundos.\n", runs, time[1]);
  }
  */
}

uint out_mincut_capacity(output out){
  uint cap=0, i, j, k, y;
  for(i=0; i<out->cutminimal->end;i++){
    j=out->cutminimal->tail[i];
    for(k=0; (y=in_neighbs_forward_menos(j,out->cutminimal,&k,out->net)); k++){
      cap+=f(j,y,out->net);
    }
  }
  return cap;
}

void printpathsek(output out, int num){
  uint i;
  
  for(i=0; i<out->n_paths; i++)
    printpathn(out->net,out->paths[i]);
}


void printpathn(Net net, path path){
  int i;

  printf("s.");
  for(i=path->n_nodes-2;i>0;i--){
    if(path->nodes[i].balance){
      printf("%u.",net->ids[path->nodes[i].name]);
    }
    else
      printf("<%u.",net->ids[path->nodes[i].name]);
  }
  printf("t:%u\n", path->flow);
}

void print_ncutminimal(output out){
  uint i;
  for(i=out->cutminimal->end-1; i>=1; i--)
    printf("%u,",out->net->ids[out->cutminimal->tail[i]]);
  if(out->cutminimal->end>0)
    printf("%u",out->net->ids[out->cutminimal->tail[0]]);
  printf("}\n");
}


#define EDGES_RESERVED 1000
#define NODES_RESERVED 1000



Net net_new() {
  Net net = NULL;
  uint edges_reserved = EDGES_RESERVED,
    nodes_reserved = NODES_RESERVED;

  net = calloc(1, sizeof(struct net) 
	       + nodes_reserved * sizeof(node));
  net->n_xplusy = calloc(edges_reserved, 
			 sizeof(usint));
  net->edges = calloc(edges_reserved, sizeof(edge));
  net->ids = malloc(nodes_reserved * sizeof(uint));
  net->n_nodes = 2;
  net->n_edges = 0;
  
  net->ids[0] = 0;
  net->ids[1] = 1;

  return net;
}


uint reserve_more_nodes(Net *netp, uint n) {
  Net net = *netp;
  uint tmp = n;
  n *= 2;

  net = realloc(net, sizeof(struct net)
		+ n * sizeof(node));
  memset(&net->nodes[tmp], 0, 
	 (n-tmp) * sizeof(node));
  net->ids = realloc(net->ids, n * sizeof(uint));

  *netp = net;

  return n;
}

uint reserve_more_edges(Net *netp, uint n) {
  uint tmp = n;
  Net net = *netp;
  n *= 2;

  net->edges = realloc(net->edges, n * sizeof(edge));
  net->n_xplusy = realloc(net->n_xplusy, 
			  n * sizeof(usint));
  memset(&net->edges[tmp], 0, 
	 (n - tmp) * sizeof(edge));
  memset(&net->n_xplusy[tmp], 0, 
	 (n - tmp) * sizeof(usint));
  *netp = net;

  return n;
}

/**
 * @brief Lee los datos del network desde stdin 
 y los guarda en un Net.  
 * @see net_new, reserve_more_nodes, reserve_more_edges
 * @returns Un nuevo Net que contiene 
 todos los datos leidos.
 **/

Net read_data() {
  Net net = NULL;
  int readed = 0;
  uint edges_reserved = EDGES_RESERVED,
    edges_tmp = 0,
    nodes_reserved = NODES_RESERVED,
    x = 0,y = 0,c = 0;
  
  // freopen ("input", "r", stdin);

  net = net_new();

  while ((readed=scanf("%u %u %u\n", &x, &y, &c))!=EOF 
	 && readed == 3 ){

    net_add_nodes(net, &x, &y);
    net_add_edge(net, x, y, c);
    if (net->n_nodes+1 >= nodes_reserved){
      nodes_reserved = 
	reserve_more_nodes(&net, nodes_reserved);
    }
    if (net->n_nodes *2+2 >= edges_reserved){
      edges_reserved = 
	reserve_more_edges(&net, edges_reserved);
    }
  }

  if(readed < 3 && readed >= 0){
    net_destroy(net);
    net = NULL;
  }
  
  return net;
}

uint in_neighbs_forward_menos(uint x,queue Q,uint *i,Net net){
  uint j=*i, nvec=net->nodes[x].n_neighbs_forw, vec;
  
  for(; j<nvec;j++){
    vec=net->nodes[x].neighbs_forw[j];
    if(!((1 << (vec&31))&Q->set[vec >> 5])){
      *i=j;
      return vec; 
    }
  }
  
  return 0;
}

uint f(uint x, uint y, Net net ){
  uint i, maxxy=net->n_xplusy[x+y];
  
  for(i=0; i<maxxy; i++){
    if(net->edges[x+y][i].x==x){
      return net->edges[x+y][i].f;
    }
    if (net->edges[x+y][i].x==y){
      return net->edges[x+y][i].f;
    }
  }

  return 0;
}

uint C(uint x, uint y, Net net ){
  uint i, maxxy=net->n_xplusy[x+y];
  
  for(i=0; i<maxxy; i++){
    if(net->edges[x+y][i].x==x){
      return net->edges[x+y][i].C;
    }
    if (net->edges[x+y][i].x==y){
      return net->edges[x+y][i].C;
    }
  }
  return 0;
}

void setf(uint x, uint y, Net net ,uint f){
  uint i, maxxy=net->n_xplusy[x+y];
  
  for(i=0; i<maxxy; i++){
    if(net->edges[x+y][i].x==x){
      net->edges[x+y][i].f=f;
      break;
    }
    if (net->edges[x+y][i].x==y){
      net->edges[x+y][i].f=f;
      break;
    }
  }
}

output createout(){
  output out;

  out=calloc(1,sizeof(struct output));

  return out;
}

path path_new(){
  path path;
  path=malloc(sizeof(struct path)+sizeof(struct snode));
  path->n_nodes=0;
  return path;
}

void path_destroy(path p){
  free(p);
}


void path_add_node (path *Sp, uint i, uint b){
  path S=*Sp;
  S->n_nodes++;
  *Sp=realloc(S,sizeof(struct path)+(S->n_nodes) * sizeof (struct snode));
  S=*Sp;
  S->nodes[S->n_nodes-1].balance=b;
  S->nodes[S->n_nodes-1].name=i;
}

void out_add_mincut(output out, queue Q){
  out->cutminimal=Q;
}

uint net_neighb_forw(Net net, uint x){
  uint i, vec=0;
  i=net->nodes[x].n_start_forw;
  while(i<net->nodes[x].n_neighbs_forw){
    vec=net->nodes[x].neighbs_forw[i];
    if (net->nodes[x].level < net->nodes[vec].level && f(x,vec,net)<C(x,vec,net))
      break;
    else {
      i=++net->nodes[x].n_start_forw;
      vec=0;
    }
  }
  return vec;
}

uint net_neighb_back(Net net, uint x){
  uint i, vec=0;
  i=net->nodes[x].n_start_back;
  while(i<net->nodes[x].n_neighbs_back){
    vec=net->nodes[x].neighbs_back[i];
    if (net->nodes[x].level < net->nodes[vec].level && f(vec,x,net)>0)
      break;
    else {
      i=++net->nodes[x].n_start_back;
      vec=0;
    }
  }
  return vec;
}

void net_del_neighb(Net net, uint x, uint bal){
  if(bal)
    net->nodes[x].n_start_forw++;
  else
    net->nodes[x].n_start_back++;
}

void print_pathsdinic(output out,int flags){
  uint i;
  

  if(flags&NUMERICO){
    for(i=0; i<out->n_pathsNA; i++){
      printpathn(out->net,out->pathsNA[i]);
    }
  }
}

void addpathdinic(path S, output out){
  out->n_pathsNA++;
  out->pathsNA=realloc(out->pathsNA, sizeof(path) * out->n_pathsNA); //path es struct path*
  out->pathsNA[out->n_pathsNA-1]=S;
}

queue_bfs queue_bfs_new(Net net){
  queue_bfs result;
  result=malloc(sizeof(struct queue_bfs) + net->n_nodes*sizeof(struct bfs_elem));
  result->start=0;
  result->end=1;
  result->size=net->n_nodes;
  result->tail[0].id=0;
  net->nodes[0].level=0;

  return result;
}

void queue_bfs_add(Net net, queue_bfs *Qq, uint i,  uint level){
  queue_bfs Q=*Qq;

  if (net->nodes[i].level==(uint)-1){
    Q->tail[Q->end].id=i;
    Q->end++;
    net->nodes[i].level=level;
    //    printf("agrego a %i a la queue. level=%u,\n", net->ids[i], level);
  }
}

void queue_bfs_add_neighbs(queue_bfs *Qq, Net net, uint i){
  queue_bfs Q=*Qq;
  uint j, k, in=qbfs_id(Q,i);
  
  for (j=0;j<net->nodes[in].n_neighbs_forw;j++){
    Q=*Qq;
    k=net->nodes[in].neighbs_forw[j];
    if(f(in,k,net)<C(in,k,net))
      queue_bfs_add(net, Qq, k,net->nodes[in].level+1);
  }

  for (j=0;j<net->nodes[in].n_neighbs_back;j++){
    Q=*Qq;
    k=net->nodes[in].neighbs_back[j];
    if(f(k,in,net)>0&&k!=in)
      queue_bfs_add(net, Qq, k, net->nodes[in].level+1);
   }
}

void net_reset_start(Net net){
  uint i;
 
  for(i=0;i<net->n_nodes;i++){
    net->nodes[i].n_start_forw=0;
    net->nodes[i].n_start_back=0;
  }
}

void net_aux_new(Net net){
  uint i;
  queue_bfs Q=queue_bfs_new(net);

  for(i=qbfs_start(Q);qbfs_not_empty(Q)&&net->nodes[qbfs_id(Q,i)].level<net->nodes[1].level;i=++qbfs_start(Q))
    queue_bfs_add_neighbs(&Q,net,i);
  
  net_reset_start(net);/*Resetea n_start_forw y n_start_back*/
  
  queue_bfs_destroy(Q);
}

void net_aux_reset(Net net){
  uint i;

  for(i=1; i<net->n_nodes; i++)
    net->nodes[i].level=(uint)-1;
}

void out_path_destroy(output out){
  uint i;
  for(i=0; i<out->n_pathsNA;i++)
    free(out->pathsNA[i]);
  free(out->pathsNA);  
  out->pathsNA=NULL;
  out->n_pathsNA=0;
}



int dinic(Net net, output*outp, int times, int flags){
  uint v=0, x=0, y=0, r=0, end=0;
  int i=0; 
  bool stop_flag = false;
  output out=*outp;
  path p = NULL;
  
  if(!out) out = createout();

  out->net=net;

  for(i=0; !end ; i++){
    v = 0;
    stop_flag = false; 
    net_aux_reset(net);
    net_aux_new(net);
    end = net->nodes[1].level == (uint)-1;
    while(!stop_flag && !end){
      p = path_new();
      x = 0;

      while(( x != 1) && !stop_flag){
	if ((y = net_neighb_forw(net,x))){/*Avanzar en vecinos forward*/
	  set_balance(net,y,1);
	  set_ancestor(net,y,x);
	  x = y;
	}
	else if ((y = net_neighb_back(net,x))){/*Avanzar en vecinos backward*/
	  set_balance(net,y,0);
	  set_ancestor(net,y,x);
	  x = y;
	}
	else if (x){//x!=s /*Retroceder*/
	  y = ancestor(net,x);
	  net_del_neighb(net,y,balance(net,x));
	  x = y;
	}
	else stop_flag = true;
      }


      if (x == 1) {/*Aumentar*/
    	y = 1,r = -1; // -1 porque es unsigned asi que -1 es el maximo valor
	while(y != 0){
	  x = ancestor(net,y);
	  if(balance(net,y)){
	    r = minx(C(x,y,net)-f(x,y,net),r);
	  }
	  else{
	    r= minx(f(y,x,net),r);
	  }
	  y = x;
	}
	
	y = 1, v += r;
	

	while (y != 0){
	  x = ancestor(net,y);
	  path_add_node(&p,y,balance(net,y));

	  if (balance(net,y)){
	    setf(x,y,net,f(x,y,net)+r);

	    if(f(x,y,net) == C(x,y,net)){
	      net_del_neighb(net,x,1);
	    }
	  }
	  else {
	    setf(y,x,net,f(y,x,net)-r);
	    if(f(y,x,net) == 0){
	      net_del_neighb(net,x,0);
	    }
	  }
	  y = x;
	}
	path_add_node(&p,y,1);
	path_set_flow(p,r);
	addpathdinic(p,out);
      }
    }
  
    path_destroy(p);
    out->flow += v;
    

    if((flags&VERBOSO) ){
      if (i>1)
      printf("PATHS DEL NETWORK AUXILIAR %u\n", i+1);
      else if (i==1)
	printf("PATHS DEL SEGUNDO NETWORK AUXILIAR\n");
      else 
	printf("PATHS DEL PRIMER NETWORK AUXILIAR\n");
      
      if(!end) print_pathsdinic(out,flags);
      
      if (i>1)
	printf("FIN NETWORK AUXILIAR %u\n", i+1);
      else if (i==1)
	printf("FIN DEL SEGUNDO NETWORK AUXILIAR\n");
      else
	printf("FIN DEL PRIMER NETWORK AUXILIAR\n");
    }

    net_aux_reset(net);
    if(!end){
      net_aux_new(net);
      end = (net->nodes[1].level == (uint)-1);
      net_aux_reset(net);
    }
    out_path_destroy(out);    
  }

  *outp=out;
  
  return end;
}

void queue_destroy(queue queue){
  free(queue->set);
  free(queue);
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

  network = read_data();

  if (network==NULL){
    printf("invalid input\n");
    return -1;
  }

  end=dinic(network,&out,times,flags);
  printout(out,flags, t, runs);

  return 0;
}
