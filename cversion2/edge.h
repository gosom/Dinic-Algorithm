#ifndef _EDGE_H_
#define _EDGE_H_

#include <stdbool.h>

typedef struct edge *edge;
typedef struct edges * edges_list;

typedef unsigned int uint;
typedef unsigned short int usint;

edges_list edges_new(uint n);
void edges_add(edges_list e, uint x, uint y, 
		     uint c, uint f);
bool edges_is_full(edges_list e);
uint edges_flow(edges_list e, uint x, uint y);
uint edges_capacity(edges_list e, uint x, uint y);
void edges_destroy(edges_list e);
void edges_realloc(edges_list e, uint size);
void edges_update_flow(edges_list e, uint x, uint y, 
		       uint f);

#endif
