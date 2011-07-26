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
#include "output.h"
#include "network.h"
#include "utils.h"



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
  
  nodes_list nodes = NULL;
  edges_list edges = NULL;

  freopen ("../pyversion/tests/networks/net02.txt", "r", stdin);

  net = net_new();
  nodes = net_get_nodes(net);
  edges = net_get_edges(net);

  while ((readed=scanf("%u %u %u\n", &x, &y, &c))!=EOF 
	 && readed == 3 ){

    net_add_nodes(net, &x, &y);
    net_add_edge(net, x, y, c);
    if (nodes_get_length(nodes)+1 
	>= nodes_reserved){
      nodes_reserved *= 2;
      nodes_realloc(nodes, nodes_reserved);
    }
    if (nodes_get_length(nodes)*2+2 
	>= edges_reserved){
      edges_reserved = edges_reserved * 2 + 2;
      edges_realloc(edges, edges_reserved);
    }
  }


  if(readed < 3 && readed >= 0){
    net_destroy(net);
    net = NULL;
  }
  
  return net;
}




int dinic(Net net, output * outp, int flags){
  uint v = 0, x = 0, y = 0, r = 0, end = 0;
  uint t = 1;
  int i = 0; 
  bool stop_flag = false;
  output out=*outp;
  path p = NULL;
  nodes_list nodes = NULL;
  edges_list edges = NULL;

  nodes = net_get_nodes(net);
  edges = net_get_edges(net);

  if(!out) out = out_new();

  out_set_net(out, net);

  for(i = 0; !end ; i++){

    v = 0;
    stop_flag = false; 
    nodes_aux_reset(nodes);
    net_aux_new(net);


    end = (nodes_get_level(nodes, t) == (uint)-1);
    while(!stop_flag && !end){
    
      p = path_new();
      x = 0;

      while(( x != 1) && !stop_flag){

	if ((y = net_neighb_forw(net, x))){
	  nodes_set_balance(nodes, y, 1);
	  nodes_set_ancestor(nodes, y, x);
	  x = y;
	}
	else if ((y = net_neighb_back(net, x))){
	  nodes_set_balance(nodes, y, 0);
	  nodes_set_ancestor(nodes, y, x);
	  x = y;
	}
	else if (x){
	  y = nodes_get_ancestor(nodes, x);
	  nodes_del_neighb(nodes, y, 
			 nodes_get_balance(nodes, x));
	  x = y;
	}
	else stop_flag = true;
      }



      if (x == 1) {/*Aumentar*/
    	y = 1,r = -1; 
	while(y != 0){
	  x = nodes_get_ancestor(nodes, y);
	  if(nodes_get_balance(nodes, y)){
	    r = MIN(edges_capacity(edges, x, y)
		     -edges_flow(edges, x, y), r);
	  } else {
	    r= MIN(edges_flow(edges, y, x), r);
	  }
	  y = x;
	}
	
	y = 1, v += r;

	while (y != 0){
	  x = nodes_get_ancestor(nodes, y);
	  path_add_node(p, y, 
			nodes_get_balance(nodes, y));

	  if (nodes_get_balance(nodes, y)){
	    edges_update_flow(edges, x, y, 
			      edges_flow(edges, x, y) 
			      + r);

	    if(edges_flow(edges, x, y) 
	       == edges_capacity(edges, x, y)){
	      nodes_del_neighb(nodes, x, 1);
	    }
	  } 
	  else {
	    edges_update_flow(edges, y, x, 
			      edges_flow(edges, y, x)
			      - r);
	    if(edges_flow(edges, y, x) == 0){
	      nodes_del_neighb(nodes, x, 0);
	    }
	  }
	  y = x;
	}
	path_add_node(p, y, 1);
	path_update_flow(p, r);
	out_add_path(p, out);
      }
      path_destroy(p);
    }
  
    out_set_flow(out, out_get_flow(out)+v);

    if((flags&VERBOSE) ){
      if(!end) print_paths(out, flags, i+1);
    }

    nodes_aux_reset(nodes);
    if(!end){
      net_aux_new(net);
      end = (nodes_get_level(nodes, t) == (uint)-1);
      nodes_aux_reset(nodes);
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

int main(int argc, char ** argv){
  Net network = NULL;
  int flags = 0, runs = 0;
  output out = NULL;

  flags = check_options(argc, argv);

  network = read_data();

  if (network == NULL){
    printf("invalid input\n");
    return -1;
  }

  dinic(network, &out, flags);
  print_output(out,flags);

  return 0;
}

