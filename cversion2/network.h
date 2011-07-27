#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "queue_bfs.h"
#include "node.h"
#include "edge.h"
#include "utils.h"

typedef struct net * Net;


Net net_new(); 
nodes_list net_get_nodes(Net net);
edges_list net_get_edges(Net net);
void net_destroy(Net net);

uint net_neighb_forw(Net net, uint x); 
uint net_neighb_back(Net net, uint x);


void net_aux_new(Net net); 

queue_bfs net_queue_bfs_new(Net net);
void print_adj_table(Net net);
void print_flow_table(Net net);
uint net_add_node(Net network, uint n); 
void net_add_nodes(Net network, uint *x, uint *y);
void net_add_edge(Net network, uint x,uint y, uint C);
void net_queue_bfs_add_neighbs(queue_bfs *Qq, 
							   Net net, 
							   uint in);


#endif
