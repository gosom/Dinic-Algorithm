#include <stdio.h>
#include <stdlib.h>
#include "network.h"
#include "output.h"


struct output{
	path *paths;
	uint n_paths;
	uint flow;
	/*  queue cutminimal;*/
	Net net;
};


uint out_num_paths(output out) {
	return out->n_paths;
}

path out_get_path(output out, uint i) {
	return out->paths[i];
}

void print_paths(output out,int flags, int na){
	uint i;
	uint flow = 0;
	uint tot_flow = 0;

	printf("N.A. %d:\n", na);
	for(i=0; i<out->n_paths; i++){
		nodes_print_path(net_get_nodes(out->net), 
				   out->paths[i], &flow);
		tot_flow += flow;
	}
	printf("El N.A. %d aumenta el flujo en %u.\n\n", 
		   na, tot_flow);
}



void out_add_path(path p, output out){
	out->n_paths++;
	out->paths = realloc(out->paths, sizeof(path) * out->n_paths); 
	out->paths[out->n_paths-1] = p;
	out->flow += path_flow(p);
}



void print_output(output out, int flags){

	if ((flags&FLUJO))
		net_print_flow_table(out->net);

	printf("Valor del flow: %i\n", out->flow);

	if ((flags&CORTE))
		out_print_cut(out);
}



output out_new(){
	output out;

	out = calloc(1,sizeof(struct output));

	return out;
}


void out_destroy(output out) {
	out_path_destroy(out);
	free(out->paths);
	free(out);
	out = NULL;
}

void out_path_destroy(output out){
	uint i;
  	for(i = 0; i < out->n_paths; i++) {
		path_destroy(out->paths[i]);

	}
	free(out->paths);
  	out->paths = NULL;
  	out->n_paths = 0;
}

void out_set_net(output out, Net net) {
	out->net = net;
}

void out_set_flow(output out, uint flow) {
	out->flow = flow;
}

uint out_get_flow(output out) {
	return out->flow;
}


void out_print_cut(output out) {
	Net net = out->net;
	queue_bfs q =  NULL;
	uint x = 0;
	nodes_list nodes = net_get_nodes(net);

	nodes_aux_reset(nodes);

	q = net_queue_bfs_new(net);
	
	printf("Cut={");

	do {
		x = queue_bfs_pop(q);
    
		if (x)
			printf(", %u", x);
		else
			printf("%u", x);

		net_queue_bfs_add_neighbs(net, &q, x);
    
	} while (!queue_bfs_is_empty(q));
  
	printf("}\n");

	queue_bfs_destroy(q);
}
