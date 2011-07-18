#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "glib.h"
#include "auxiliar_network.h"
#include "vertex.h"
#include "parser.h"
#include "edge.h"
#include "path.h"


edge get_edge(edges_list edges, int first, int last) {
  int i = 0, edge_length=0;
  edge e = NULL;
  GSList * edge_data = NULL;
  
  edge_length = g_slist_length(edges);
  edge_data = edges;
  for (i = 0; i < edge_length; i++, edge_data=g_slist_next(edge_data)) {
    e = edge_data->data;
    if (edge_first(e) == first 
	&& edge_last(e) == last){
      return e;
    }
  }
  return NULL;
}

int get_residual_capacity(int father, vertex child, 
			  edges_list edges) {
  edge e = NULL;
  int r = 0;
  assert(edges != NULL);

  if (vertex_direction(child) == true) { /* Fordward */
    e = get_edge(edges, father, vertex_id(child));
    assert(e != NULL);
    r = edge_capacity(e) - edge_flow(e);
  } else  {
    e = get_edge(edges, vertex_id(child), father);
    assert(e != NULL);
    r = edge_flow(e);
  }
  
  return r;
}



path get_path(aux_net an, edges_list edges, 
	      int * minflow, bool * complete) {
  
  path p = NULL;
  int s = 0, t = 1, r = 0;
  vertex n = NULL;
  int key = 0; /* s node */
  
  *minflow = INT_MAX;
  *complete = false;
  n = make_vertex(s, true); /* Source Node (s)*/
  p = g_queue_new();
  g_queue_push_tail(p, n);

  while (true) {

    n = an_get_child(an, key);
    if (n != NULL) {
      g_queue_push_tail(p, n);
    } else if (key == s) {
      break;
    } else if (key == t) {
      *complete = true;
      break;
    } else {
      g_queue_pop_tail(p);
      n = g_queue_peek_tail(p);
      an_remove_edges_entering(an, key);
      key = vertex_id(n);
      continue;
    }
    r = get_residual_capacity(key, n, edges);
    if (r < *minflow) {
      *minflow = r;
    }
    key = vertex_id(n);
  }

  return p;
}


void augment_and_delete(aux_net an, edges_list edges,
			vertex v, int first, int last, 
			int mincost) {
  edge e = NULL;

  assert(v != NULL);
 
  if (vertex_direction(v) == true) {
    e = get_edge(edges, first, last);
    update_flow(e, edge_flow(e)+mincost);
    if (edge_capacity(e)-edge_flow(e) == 0) {
      an_remove_edge(an, edge_first(e), edge_last(e));
    }
  } else {
    e = get_edge(edges, last, first);
    update_flow(e, edge_flow(e)-mincost);
    if (edge_flow(e) == 0) {
      an_remove_edge(an, edge_last(e), edge_first(e));
    }
  }

}


void augment(aux_net an, edges_list edges, 
	     path p, int mincost) {
  
  int first = 0; /* s is the first node in the path */
  int last = 0;
  vertex v = NULL;

  assert(an != NULL);
  assert(edges != NULL);
  assert(p != NULL);

  g_queue_pop_head(p); /* remove vertex s */

  while (!g_queue_is_empty(p)) {
    v = g_queue_pop_head(p);
    last = vertex_id(v);
    augment_and_delete(an, edges, v, first, last, 
		       mincost);
    first = last;
  }
  
}


void print_path(path p) {
  int i = 0;
  vertex v = NULL;

  for (i = 0; i < g_queue_get_length(p); i++) {
    v = g_queue_peek_nth(p, i);
    printf("%d ", vertex_id(v));
  }
  printf("\n");
}