#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include "edge.h"
#include "parser.h"
#include "path.h"
#include "auxiliar_network.h"


void reset_edges(edges_list edges) {
  int i = 0, edges_length = 0;
  edge e = NULL;
  GSList * edge_data = NULL;
  
  edges_length = g_slist_length(edges);
  edge_data = edges;
  for (i = 0; i < edges_length; i++, edge_data=g_slist_next(edge_data)) {
    e = edge_data->data;
    edge_set_used(e, false);
  }
}


GSList * get_corte(aux_net an) {
  GSList * corte = NULL;
  int s = 0, i = 0;
  GHashTableIter iter;
  gpointer key, value;
  vertex v = NULL;

  g_hash_table_iter_init(&iter, an);
  corte = g_slist_prepend(corte, make_int(s));
  
  while (g_hash_table_iter_next(&iter, &key, &value)) {
    for (i = 0; i < g_slist_length(value); i++) {
      v = g_slist_nth_data(value, i);
      corte = g_slist_prepend(corte, v);
    }
  }

  return corte;
}

int calculate_maxflow(edges_list edges) {
  int maxflow = 0, i = 0, s = 0;
  edge e = NULL;

  for (i = 0; i < g_slist_length(edges); i++) {
    e = g_slist_nth_data(edges, i);
    if (edge_first(e) == s) {
      maxflow += edge_flow(e);
    }
  }

  return maxflow;
}


int main(int argc, char ** argv) {
  edges_list edges = NULL;
  aux_net an = NULL;
  bool an_complete = false, path_complete = false;
  int minflow = 0, maxflow = 0;
  path p = NULL;
  GSList * corte = NULL;

  open_file("../pyversion/tests/networks/complex5000.txt"); 
  /*  open_file("../pyversion/tests/networks/net02.txt");*/

  edges = read_edges();
  /*print_edges(edges); */
  while (true) {
    reset_edges(edges);

    an = make_auxiliar_network(edges, &an_complete);
    /*    an_print(an);*/

    if (!an_complete) break;
    while (true) {
      p = get_path(an, edges, &minflow, 
		   &path_complete);
      if (path_complete) {
	print_path(p); 
	printf("minflow: %d\n", minflow);
	augment(an, edges, p, minflow);
      } else {
	break;
      }
    }
  }
  corte = get_corte(an);
  maxflow = calculate_maxflow(edges);
  printf("MaxFlow: %d\n", maxflow);
  destroy_edges(edges);

  return 0;
}
