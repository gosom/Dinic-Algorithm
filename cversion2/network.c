#include <stdio.h>
#include <stdlib.h>

#include "network.h"
#include "queue_bfs.h"

struct net {
	nodes_list nodes;
	edges_list edges;
};


nodes_list net_get_nodes(Net net) {
	return net->nodes;
}

edges_list net_get_edges(Net net) {
	return net->edges;
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


void net_destroy(Net net){
	edges_destroy(net->edges);
	nodes_destroy(net->nodes);
	free(net);
}  





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

	nodes_reset_start(net->nodes);
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
			nodes_queue_bfs_add(net->nodes, Qq, k, 
								nodes_get_level(net->nodes, in)+1);

		}
	}
	j = 0;
	max = nodes_back_get_length(net->nodes, in);
	for (i = j; i<max; i++) {
		k = nodes_nth_back_neighb(net->nodes, in, i);
		if (edges_flow(net->edges, k, in) > 0 
			&& k != in) {
			nodes_queue_bfs_add(net->nodes, Qq, k, 
								nodes_get_level(net->nodes, in)+1);

		}
	}
}
