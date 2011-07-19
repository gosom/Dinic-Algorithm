#ifndef EDGE_H
#define EDGE_H

#include "queue.h"

typedef struct sedge * edge;
typedef queue edges_list;
typedef void (*free_edge_func) (void*);

edge make_edge(int first, int last, int capacity, 
	       int flow, bool used);

void destroy_edge(edge e);
int edge_first(edge e);
int edge_last(edge e);
int edge_capacity(edge e);
int edge_flow(edge e);
void update_flow(edge e, int flow);
bool edge_used(edge e);
void edge_set_used(edge e, bool used);
void print_edge(edge e);

edges_list read_edges();
void print_edges(edges_list);

void destroy_edges(edges_list edges);

#endif
