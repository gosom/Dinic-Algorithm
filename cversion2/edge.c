#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "edge.h"

struct edge{
  uint x:14;
  uint y:14;
  uint c:31;
  uint f:31;
};


struct edges {
  uint n_edges;
  uint size;
  edge *edges_list;
  usint * n_xplusy;
};

edges_list edges_new(uint n) {
  edges_list e = NULL;
  uint i;

  e = calloc(1, sizeof(struct edges));
  e->edges_list = calloc(n, sizeof(edge));
  e->n_xplusy = calloc(n, sizeof(usint));
  e->n_edges = 0;
  e->size = n;

  return e;
}

void  edges_add(edges_list e, uint x, uint y, 
		uint c, uint f) {
  uint pos, size;

  pos = e->n_xplusy[x+y];
  size = ++e->n_xplusy[x+y];

  e->edges_list[x+y] = realloc(e->edges_list[x+y],
			       sizeof(struct edge)
			       * size);
  memset(&e->edges_list[x+y][pos], 0, 
	 sizeof(struct edge));
  e->edges_list[x+y][pos].x = x;
  e->edges_list[x+y][pos].y = y;
  e->edges_list[x+y][pos].f = f;
  e->edges_list[x+y][pos].c = c;
  e->n_edges++;

}



void edges_destroy(edges_list e) {
  /*
  free(e->edges_list);
  free(e->n_xplusy);
  free(e);
  */
}



bool edges_is_full(edges_list e) {
  return (e->size == e->n_edges);
}


uint edges_flow(edges_list e, uint x, uint y) {
  uint flow = -1, i;


  for (i = 0; i < e->n_xplusy[x+y]; i++) {
    if (e->edges_list[x+y][i].x == x 
	&& e->edges_list[x+y][i].y == y) {
      flow = e->edges_list[x+y][i].f;
      break;
    }
  }

  return flow;
}


uint edges_capacity(edges_list e, uint x, uint y) {
  uint cap = 0, i;


  for (i = 0; i < e->n_xplusy[x+y]; i++) {
    if (e->edges_list[x+y][i].x == x 
	&& e->edges_list[x+y][i].y == y)
      cap = e->edges_list[x+y][i].c;
  }

  assert(cap >= 0);

  return cap;
}

void edges_realloc(edges_list e, uint size) {
  uint i;

  e->edges_list = realloc(e->edges_list, 
			  size * sizeof(edge));
  memset(&e->edges_list[e->size], 0, (size - e->size)
	 * sizeof(edge));
  e->n_xplusy = realloc(e->n_xplusy, 
			size * sizeof(usint));
  memset(&e->n_xplusy[e->size], 0, (size - e->size)
	 * sizeof(usint));
  e->size = size;
}

void edges_update_flow(edges_list e, uint x, uint y, 
		       uint f) {
  usint max, i;


  max = e->n_xplusy[x+y];

  for (i = 0; i < max; i++) {
    if (e->edges_list[x+y][i].x == x
	|| e->edges_list[x+y][i].x == y) {
      e->edges_list[x+y][i].f = f;
    }
  }

}
