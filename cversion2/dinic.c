#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <getopt.h>
#include <limits.h>
#include "queue_bfs.h"
#include "node.h"
#include "edge.h"
#include "path.h"
#include "output.h"
#include "network.h"
#include "utils.h"


/**
 * Lee los datos del network desde stdin
 * y los guarda en un Net. Opcionalmente lee los datos
 * de un archivo pasado como parametro. 
 * @see net_add_nodes, net_add_edges, nodes_realloc, edges_realloc,
 * net_get_nodes, net_get_edges, net_destroy,
 * EDGES_RESERVED, NODES_RESERVED
 * @param file_name (Opcional)El nombre del archivo desde donde se lee
 * el network, si es NULL los datos se leen desde stdin.
 * @returns Un nuevo Net que contiene los datos del network.
 todos los datos leidos.
 **/

Net read_data(char *file_name) 
{
	Net net = NULL;
	int readed = 0;
	uint edges_reserved = EDGES_RESERVED, 
		nodes_reserved = NODES_RESERVED,
		x = 0,y = 0,c = 0;
	nodes_list nodes = NULL;
	edges_list edges = NULL;
	
	if (file_name != NULL)
	freopen (file_name, "r", stdin);
	
	net = net_new();
	nodes = net_get_nodes(net);
	edges = net_get_edges(net);
	
	while ((readed=scanf("%u %u %u\n", &x, &y, &c))!=EOF
		 && readed == 3 ){
		
		net_add_nodes(net, &x, &y);
		net_add_edge(net, x, y, c);
		if (nodes_get_length(nodes)+1 >= nodes_reserved){
			nodes_reserved *= 2;
			nodes_realloc(nodes, nodes_reserved);
		}
		if (nodes_get_length(nodes)*2+2 >= edges_reserved){
			edges_reserved = edges_reserved * 2 + 2;
			edges_realloc(edges, edges_reserved);
		}
	}
	

	if(readed < 3 && readed >= 0){
		net_destroy(net);
	}

	return net;
}

uint advance(Net net,  bool * stop_flag_p) 
{
	nodes_list nodes = net_get_nodes(net);
	uint x = 0, y = 0;

	while(( x != 1) && !*stop_flag_p){

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
		else *stop_flag_p = true;
	}
	return x;
}

uint residual_capacity(Net net) 
{
	uint x = 0, y = 1, r = UINT_MAX;
	nodes_list nodes = net_get_nodes(net);
	edges_list edges = net_get_edges(net);

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
	return r;
}


void augment(Net net, path p, uint flow) 
{
	uint x = 0, y = 1;
	nodes_list nodes = net_get_nodes(net);
	edges_list edges = net_get_edges(net);

	while (y != 0) {
		x = nodes_get_ancestor(nodes, y);
		path_add_node(p, y, nodes_get_balance(nodes, y));
		
		if (nodes_get_balance(nodes, y)) {
			edges_update_flow(edges, x, y, edges_flow(edges, x, y) 
							  + flow);
			
			if(edges_flow(edges, x, y) == edges_capacity(edges, x, y)){
				nodes_del_neighb(nodes, x, 1);
			}
		}
		else {
			edges_update_flow(edges, y, x, edges_flow(edges, y, x) 
							  - flow);
			if(edges_flow(edges, y, x) == 0) {
				nodes_del_neighb(nodes, x, 0);
			}
		}
		y = x;
	}
	path_add_node(p, y, 1);
	path_update_flow(p, flow);
}


bool is_t_in_an(nodes_list n) 
{
	uint t = 1;
	return (nodes_get_level(n, t) == UINT_MAX);
}

void dinic(Net net, output * out_p, int flags){
	uint an_flow = 0, x = 0, r = 0, t = 1, i = 0;
	bool stop_flag = false, end = false;
	output out = *out_p;
	path p = NULL;
	nodes_list nodes = NULL;

	nodes = net_get_nodes(net);
	out_set_net(out, net);

	for(i = 0; !end ; i++) {

		an_flow = 0;
		stop_flag = false;

		nodes_aux_reset(nodes);
		net_aux_new(net);

		end = is_t_in_an(nodes);

		while(!stop_flag && !end) {

			x = advance(net, &stop_flag); /* ADVANCE */

			if (x == t) { /* AUGMENT */
				r = residual_capacity(net); 
				an_flow += r;
				p = path_new();
				augment(net, p, r);
				out_add_path(p, out);
			} 
		}

		if((flags & VERBOSE) && !end){
			print_paths(out, flags, i+1);
		}

		out_path_destroy(out);
	}
	*out_p = out;
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

int check_options(int argc, char **argv) 
{
	int c = 0;
	int flags = 0;

	while (true) {
		int option_index = 0;

		static struct option long_options[] = {
			{"flujo", no_argument, 0, 'f'},
			{"verbose", no_argument, 0, 'v'},
			{"corte", no_argument, 0, 'c'},
			{0, 0, 0, 0}
		};


		c = getopt_long(argc, argv, "fvc", long_options, &option_index);

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

int main(int argc, char ** argv)
{
	Net network = NULL;
	int flags = 0;
	output out = NULL;
	char * file_name = "net02.txt";

	flags = check_options(argc, argv);
	network = read_data(file_name);
	out = out_new();

	if (network == NULL){
		printf("invalid input\n");
		return -1;
	}

	dinic(network, &out, flags);
	print_output(out,flags);
	out_destroy(out);
	net_destroy(network);

	return 0;
}
