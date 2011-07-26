#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <getopt.h>
#include "queue_bfs.h"
#include "node.h"
#include "edge.h"
#include "path.h"

typedef struct output * output;

typedef unsigned int uint;
typedef unsigned short int usint;

typedef struct net * Net;

struct net {
  nodes_list nodes;
  edges_list edges;
};

  
struct output{
  path *pathsNA; /*Agregado para la impresion de Dinic*/
  uint n_pathsNA; /*Agregado para la impresion de Dinic*/
  uint n_paths;
  uint flow;
  /*  queue cutminimal;*/
  path * paths;
  Net net;
  uint ttime;
};

#define VERBOSE 1
#define FLUJO 2
#define CORTE 4
#define NO_VALID_ARG 8

#define SOURCE_ID 0
#define TARGET_ID 1

#define EDGES_RESERVED 1000
#define NODES_RESERVED 1000

#define MULT_NODES_RESERVED 2
#define MULT_EDGES_RESERVED 2


Net net_new(); 

void net_destroy(Net net);

uint net_neighb_forw(Net net, uint x); 
uint net_neighb_back(Net net, uint x);
void print_adj_table(Net net);

void net_aux_new(Net net); 

void net_del_neighb(nodes_list n, uint x, uint bal);


void queue_bfs_destroy(queue_bfs q);/* usada ninguna */
void out_path_destroy(output out); /* usada ninguna */

void net_queue_bfs_add(nodes_list nodes, 
		       queue_bfs *Qq, uint i, 
		       uint level); /* usada ninguna */
queue_bfs net_queue_bfs_new(Net net);/* usada ninguna */
void net_queue_bfs_add_neighbs(queue_bfs *Qq, 
			       Net net, 
			       uint i);/* usada completa */


void print_pathsdinic(output out,int flags, int na); /* usada completa */
void addpathdinic(path S, output out); /* usada ninguna */

uint out_mincut_capacity(output out); /*usada completa*/
int dinic(Net net, output * outp, int flags); /* usada completa */

void printpathn(Net net, path path, uint *flowp); /*usada ninguna */
/*
void print_ncutminimal(output out);
*/
/*
void out_add_mincut(output out, queue q);
*/
void printout(output out, int flags, int time[], int runs); /* usada completa */



void print_flow_table(Net net);

uint minx(uint x, uint y);  /* usada completa */


int check_args(int argc, char ** argv, int *times, int *ntimes); /* usada completa */

uint net_add_node(Net network, uint n); /*usada*/

void net_add_nodes(Net network, uint *x, uint *y);/*usada*/

output createout(void); /* usada ninguna */

int main(int argc, char ** argv);


/** 
 * Destruye la estructura Net, liberando todos los recursos.
 * @param net la estructura a destruir.
 */



uint net_neighb_forw(Net net, uint x){
  uint y = 0, i;
  uint n_forw = nodes_forw_get_length(net->nodes, x);
  uint start_forw=nodes_forw_get_start(net->nodes, x);

  for (i = start_forw; i < n_forw; i++) {
    y = nodes_nth_forw_neighb(net->nodes, x, i);
    if (nodes_get_level(net->nodes, x) 
	< nodes_get_level(net->nodes, y)
	&& edges_flow(net->edges, x, y) 
	< edges_capacity(net->edges, x, y)) {
      break;
    }

    else  {
      nodes_del_forw(net->nodes, x);
      y = 0;
    }
  }
  
   return y;
}



uint net_neighb_back(Net net, uint x){
  uint y = 0, i;
  uint n_back = nodes_back_get_length(net->nodes, x);
  uint start_back=nodes_back_get_start(net->nodes, x);

  for (i = start_back; i < n_back; i++) {
    y = nodes_nth_back_neighb(net->nodes, x, i);
    if (nodes_get_level(net->nodes, x) 
	< nodes_get_level(net->nodes, y)
	&& edges_flow(net->edges, y, x) > 0)
      break;
    else  {
      nodes_del_back(net->nodes, x);
      y = 0;
    }
  }
  
  return y;
}





void net_destroy(Net net){
  edges_destroy(net->edges);
  nodes_destroy(net->nodes);
  free(net);
}  


/**
 * Devuelve el menor de sus argumentos.
 * @param x entero sin signo.
 * @param y entero sin signo.
 */

uint minx (uint x, uint y){
  return (x<y?x:y);
}

/**
 * Agrega un nodo al network.
 * @param net El network.
 * @param n Puntero al id del nodo a agregar.
 * @returns n Devolvemos en n la posicion donde fue agregado el nodo
 */

uint  net_add_node(Net net, uint id){
  uint pos = 0;
  pos = nodes_add(net->nodes, id);
  return pos;
}

/**
 * Agrega los nodos pasados como parametros.
 * @param net El network.
 * @param x Puntero al id del nodo a agregar.
 * @param y Puntero al id del nodo a agregar.
 * @returns x Puntero a la posicion donde 
 * fue agregado el nodo.
 * @returns y Puntero a la posicion donde 
 * fue agregado el nodo.
 */

void net_add_nodes(Net net, uint *x, uint *y){
  uint i;
  bool x_in_net = false,
    y_in_net = false;

  
  for(i = 0; i < nodes_get_length(net->nodes); i++){
    if(!x_in_net && nodes_get_id(net->nodes, i) 
       == *x){
      x_in_net = true;
      *x = i;
    } 
    
  }
  for(i = 0; i < nodes_get_length(net->nodes); i++){
    if(!y_in_net && nodes_get_id(net->nodes, i) == *y){
      y_in_net = true;
      *y = i;
    }
  }

  if (!x_in_net) *x = net_add_node(net, *x);
  if (!y_in_net) *y = net_add_node(net, *y);
}

/**
 * Agrega un lado al network.
 * @param network El network donde guardamos el lado.
 * @param x El primer nodo del lado.
 * @param y El segundo nodo del lado.
 * @param C La capacidad del lado.
 */

void net_add_edge(Net network, uint x,uint y, uint C){
  edges_add(network->edges, x, y, C, 0);
  nodes_add_neighbs(network->nodes, x, y);
}


void printout(output out, int flags, 
	      int time[], int runs){

  if ((flags&FLUJO))
    print_flow_table(out->net);
  /*
  if((flags&VERBOSE))
    printpathsek(out, 1);
  */
  printf("Valor del flow: %i\n",out->flow);
}


void printpathn(Net net, path p, uint *flowp){
  int i;
  nodes_list nodes = net->nodes;

  printf("0 ");
  for(i = path_length(p)-2; 0 < i ; i--){
    if(path_nth_balance(p, i)){
      printf("%u ",
	     nodes_get_id(nodes, 
			  path_nth_name(p, i)));
    } else
      printf("<- %u ",
	     nodes_get_id(nodes, 
			   path_nth_name(p, i)));
  }
  printf("1 (flujo transportado: %u)\n", path_flow(p));
  *flowp = path_flow(p);
}


/**
 * Crea un nuevo network. Reserva memoria
 * para cierta cantidad predeterminado de lados
 * y nodos.
 * @see EDGES_RESERVED, NODES_RESERVED
 * @returns El network creado.
 */

Net net_new() {
  Net net = NULL;
  uint edges_reserved = EDGES_RESERVED,
    nodes_reserved = NODES_RESERVED;

  
  net = calloc(1, sizeof(struct net) 
	       + nodes_reserved * sizeof(node));

  net->nodes = nodes_new(nodes_reserved);
  net->edges = edges_new(edges_reserved);

  nodes_set_id(net->nodes, 0, 0);
  nodes_set_id(net->nodes, 1, 1);

  return net;
}

/**
 * Lee los datos del network desde stdin 
 y los guarda en un Net.  
 * @see net_new, reserve_more_nodes, reserve_more_edges
 * @returns Un nuevo Net que contiene 
 todos los datos leidos.
 **/

Net read_data() {
  Net net = NULL;
  int readed = 0;
  uint edges_reserved = EDGES_RESERVED,
    nodes_reserved = NODES_RESERVED,
    x = 0,y = 0,c = 0;
  
  freopen ("../pyversion/tests/networks/complex5000.txt", "r", stdin);

  net = net_new();

  while ((readed=scanf("%u %u %u\n", &x, &y, &c))!=EOF 
	 && readed == 3 ){

    net_add_nodes(net, &x, &y);
    net_add_edge(net, x, y, c);
    if (nodes_get_length(net->nodes)+1 
	>= nodes_reserved){
      nodes_reserved *= 2;
      nodes_realloc(net->nodes, nodes_reserved);
    }
    if (nodes_get_length(net->nodes)*2+2 
	>= edges_reserved){
      edges_reserved = edges_reserved * 2 + 2;
      edges_realloc(net->edges, edges_reserved);
    }
  }


  if(readed < 3 && readed >= 0){
    net_destroy(net);
    net = NULL;
  }
  
  return net;
}

output createout(){
  output out;

  out = calloc(1,sizeof(struct output));

  return out;
}

void net_del_neighb(nodes_list nodes, 
		    uint x, uint bal){
  if(bal)
    nodes_del_forw(nodes, x);
  else
    nodes_del_back(nodes, x);
}

void print_pathsdinic(output out,int flags, int na){
  uint i;
  uint flow = 0;
  uint tot_flow = 0;

  printf("N.A. %d:\n", na);
  for(i=0; i<out->n_pathsNA; i++){
    printpathn(out->net, out->pathsNA[i], &flow);
    tot_flow += flow;
  }
  printf("El N.A. %d aumenta el flujo en %u.\n\n", 
	 na, tot_flow);
}

void addpathdinic(path S, output out){
  out->n_pathsNA++;
  out->pathsNA=realloc(out->pathsNA, sizeof(path) * out->n_pathsNA); 
  out->pathsNA[out->n_pathsNA-1]=S;
}

void net_aux_new(Net net){
  uint i, t = 1;
  queue_bfs Q = net_queue_bfs_new(net);
  
  do {
    i = queue_bfs_pop(Q);
    if (nodes_get_level(net->nodes, i) 
	< nodes_get_level(net->nodes, t))
      net_queue_bfs_add_neighbs(&Q, net, i);
    else break;

  } while (!queue_bfs_is_empty(Q));

  nodes_reset_start(net->nodes);/*Resetea n_start_forw y n_xstart_back*/
  queue_bfs_destroy(Q);
}


queue_bfs net_queue_bfs_new(Net net){
  queue_bfs result;
  uint s = 0;
  result = queue_bfs_new(nodes_get_length(net->nodes));
  queue_bfs_push(result, 0);
  nodes_set_level(net->nodes, s, 0);

  return result;
}


void net_queue_bfs_add(nodes_list nodes, 
		       queue_bfs *Qq, 
		       uint i,  uint level){
  queue_bfs Q=*Qq;

  if (nodes_get_level(nodes, i) == (uint)-1){
    queue_bfs_push(Q, i);
    nodes_set_level(nodes, i, level);
  }
}



void net_queue_bfs_add_neighbs(queue_bfs *Qq, 
			       Net net, 
			       uint in){
  uint i, k, j, max;
  

  j = 0;
  max = nodes_forw_get_length(net->nodes, in);

  for (i = j; i< max; i++) {
    k = nodes_nth_forw_neighb(net->nodes, in, i);
    if (edges_flow(net->edges, in, k)
	< edges_capacity(net->edges, in, k)) {
      net_queue_bfs_add(net->nodes, Qq, k, 
			nodes_get_level(net->nodes, in)+1);

    }
  }
  j = 0;
  max = nodes_back_get_length(net->nodes, in);
  for (i = j; i<max; i++) {
    k = nodes_nth_back_neighb(net->nodes, in, i);
    if (edges_flow(net->edges, k, in) > 0 
	&& k != in) {
      net_queue_bfs_add(net->nodes, Qq, k, 
			nodes_get_level(net->nodes, in)+1);

    }
  }
}



void out_path_destroy(output out){
  uint i;
  for(i=0; i<out->n_pathsNA;i++)
    free(out->pathsNA[i]);
  free(out->pathsNA);  
  out->pathsNA=NULL;
  out->n_pathsNA=0;
}

void print_nodes(Net net) {
  node n;
  uint i;
  for (i = 0; i < nodes_get_length(net->nodes); i++){
    n = nodes_get_node(net->nodes, i);
    printf("i:%u level:%u b:%u\n", i, 
	   node_get_level(n), 
	   node_get_balance(n));
      
  }
}


int dinic(Net net, output * outp, int flags){
  uint v = 0, x = 0, y = 0, r = 0, end = 0;
  uint t = 1;
  int i = 0; 
  bool stop_flag = false;
  output out=*outp;
  path p = NULL;
  
  if(!out) out = createout();

  out->net = net;
  for(i = 0; !end ; i++){

    v = 0;
    stop_flag = false; 
    nodes_aux_reset(net->nodes);
    net_aux_new(net);


    end = (nodes_get_level(net->nodes, t) == (uint)-1);
    while(!stop_flag && !end){
    
      p = path_new();
      x = 0;


      while(( x != 1) && !stop_flag){

	if ((y = net_neighb_forw(net, x))){
	  nodes_set_balance(net->nodes, y, 1);
	  nodes_set_ancestor(net->nodes, y, x);
	  x = y;
	}
	else if ((y = net_neighb_back(net, x))){
	  nodes_set_balance(net->nodes, y, 0);
	  nodes_set_ancestor(net->nodes, y, x);
	  x = y;
	}
	else if (x){
	  y = nodes_get_ancestor(net->nodes, x);
	  net_del_neighb(net->nodes, y, 
			 nodes_get_balance(net->nodes, x));
	  x = y;
	}
	else stop_flag = true;
      }



      if (x == 1) {/*Aumentar*/
    	y = 1,r = -1; 
	while(y != 0){
	  x = nodes_get_ancestor(net->nodes, y);
	  if(nodes_get_balance(net->nodes, y)){
	    r = minx(edges_capacity(net->edges, x, y)
		     -edges_flow(net->edges, x, y), r);
	  } else {
	    r= minx(edges_flow(net->edges, y, x),r);
	  }
	  y = x;
	}
	
	y = 1, v += r;

	while (y != 0){
	  x = nodes_get_ancestor(net->nodes, y);
	  path_add_node(p, y, 
			nodes_get_balance(net->nodes, y));

	  if (nodes_get_balance(net->nodes, y)){
	    edges_update_flow(net->edges, x, y, 
			      edges_flow(net->edges, x, y) 
			      + r);

	    if(edges_flow(net->edges, x, y) 
	       == edges_capacity(net->edges, x, y)){
	      net_del_neighb(net->nodes, x, 1);
	    }
	  } 
	  else {
	    edges_update_flow(net->edges, y, x, 
			      edges_flow(net->edges, y, x)
			      - r);
	    if(edges_flow(net->edges, y, x) == 0){
	      net_del_neighb(net->nodes, x, 0);
	    }
	  }
	  y = x;
	}
	path_add_node(p, y, 1);
	path_update_flow(p, r);
	addpathdinic(p, out);
      }
      path_destroy(p);
    }
  
    out->flow += v;
    

    if((flags&VERBOSE) ){
      if(!end) print_pathsdinic(out, flags, i+1);
    }

    nodes_aux_reset(net->nodes);
    if(!end){
      net_aux_new(net);
      end = (nodes_get_level(net->nodes, t) == (uint)-1);
      nodes_aux_reset(net->nodes);
    }
    out_path_destroy(out);    
  }

  *outp=out;
  
  return end;
}


/**
 * Revisa los argumentos pasados al programa 
 * si es que se paso alguno y devuelve las opciones
 * codificadas en un entero. En caso de error termina
 * la ejecucion del programa.
 * @param argc Cantidad de argumentos del programa.
 * @param argv Vector con los argumentos del programa.
 * @returns Un entero cuyo valor codifica las opciones.
 */

int check_options(int argc, char **argv) {
  int c = 0;
  int flags = 0;

  while (1) {
      int option_index = 0;

    static struct option long_options[] = {
      {"flujo", no_argument, 0, 'f'},
      {"verbose", no_argument, 0, 'v'},
      {"corte", no_argument, 0, 'c'},
      {0, 0, 0, 0}
    };

    c = getopt_long(argc, argv, "fvc", 
		    long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {

    case 'f':
      flags |= FLUJO;
      break;
      
    case 'v':
      flags |= VERBOSE;
      break;

    case 'c':
      flags |= CORTE;
      break;

    default:
      flags |= NO_VALID_ARG;
    }
  }

  if (optind < argc || flags & NO_VALID_ARG) {
    printf("\n");
    printf("usage: dinic [options]\n");
    printf("options:\n");
    printf("\t -f, --flujo \t Imprime tabla con valores del flujo.\n");
    printf("\t -v, --verbose \t Imprime informacion de los caminos.\n");
    printf("\t -c, --corte \t Imprime el corte minimal encontrado.\n");
    printf("\n");

    exit(EXIT_FAILURE);
  }

  return flags;
}

void print_flow_table(Net net) {
  uint x ,y;
  uint i;
  nodes_list nodes = net->nodes;
  edges_list edges = net->edges;

  printf("\nFlujo Maximal:\n\n");
  
  for (x = 0; x < nodes_get_length(nodes); x++) {
    for (i = 0; i<nodes_forw_get_length(nodes, x); 
	 i++){
      y = nodes_nth_forw_neighb(nodes, x, i);
      printf("%u\t%u\t%u (%u)\n", x, y,
	     edges_flow(edges, x, y),
	     edges_capacity(edges, x, y));
    }
  }
  
  printf("\n");
}

void print_adj_table(Net net) {
  uint x ,y;
  uint i;
  nodes_list nodes = net->nodes;

  printf("\nAdj table:\n\n");
  
  for (x = 0; x < nodes_get_length(nodes); x++) {
    for (i = 0; i<nodes_forw_get_length(nodes, x); 
	 i++){
      y = nodes_nth_forw_neighb(nodes, x, i);
      printf("%u\t%u\n", x, y);
    }
  }
  
  printf("\n");
}


int main(int argc, char ** argv){
  Net network = NULL;
  int flags = 0, runs = 0;
  int t[2];

  output out = NULL;

  flags = check_options(argc, argv);

  network = read_data();

  if (network == NULL){
    printf("invalid input\n");
    return -1;
  }

  dinic(network, &out, flags);
  printout(out,flags, t, runs);

  return 0;
}

