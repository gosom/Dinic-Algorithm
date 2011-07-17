#ifndef EDGE_H
#define EDGE_H

typedef struct sedge * edge;

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


#endif
