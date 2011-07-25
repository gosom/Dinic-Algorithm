#include <stdlib.h>
#include <assert.h>
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
  edge * edges_list;
  usint * n_xplusy;
};

edges_list edges_new(uint n) {
  edges_list e = NULL;

  e = calloc(1, sizeof(struct edges));
  e->edges_list = calloc(n, sizeof(edge));
  e->n_xplusy = calloc(n, sizeof(uint));
  e->size = n;

  return e;
}

void  edges_add(edges_list e, uint x, uint y, 
		uint c, uint f) {
  uint pos = e->n_xplusy[x+y];
  uint size = ++e->n_xplusy[x+y];
  e->edges_list[x+y] = realloc(e->edges_list[x+y],
			       sizeof(struct edge)
			       * size);

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
  edge p = e->edges_list[x+y];
  uint flow = -1, i;

  assert(p != NULL);

  for (i = 0; i < e->n_xplusy[x+y]; i++) {
    if (p[i].x == x && p[i].y == y) {
      flow = p[i].f;
      break;
    }
    
  }

  return flow;
}


uint edges_capacity(edges_list e, uint x, uint y) {
  edge p = e->edges_list[x+y];
  uint cap = 0, i;

  assert(p != NULL);

  for (i = 0; i < e->n_xplusy[x+y]; i++) {
    if (p[i].x == x && p[i].y == y)
      cap = p[i].c;
  }

  assert(cap >= 0);

  return cap;
}

void edges_realloc(edges_list e, uint size) {
  e->edges_list = realloc(e->edges_list, 
			  size * sizeof(edge));
  e->n_xplusy = realloc(e->n_xplusy, 
			size * sizeof(uint));
  e->size = size;

}

void edges_update_flow(edges_list e, uint x, uint y, 
		       uint f) {
  usint max = e->n_xplusy[x+y], i;

  for (i = 0; i < max; i++) {
    if (e->edges_list[x+y][i].x == x
	|| e->edges_list[x+y][i].x == y) {
      e->edges_list[x+y][i].f = f;
    }
  }
}
