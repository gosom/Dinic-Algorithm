#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "glib.h"
#include "edge.h"
#include "parser.h"


void open_file(const char * filename) {
  FILE * f = NULL;
  f = freopen(filename, "r", stdin);
  if (f == NULL) {
    printf("Failed to open file %s.\n", filename);
    exit(1);
  }
}


edges_list read_edges() {
  int first = 0, last = 0, capacity = 0;
  edge e = NULL;
  edges_list edges = NULL;

  while(scanf("%d %d %d\n", &first, &last, &capacity) != EOF){
    e = make_edge(first, last, capacity, 0, false);
    edges = g_slist_prepend(edges, e);
  }
  return edges;
}


void print_edges(edges_list edges) {
  edge e = NULL;
  int i;
  for (i=0; i<g_slist_length(edges); i++) {
    print_edge(g_slist_nth_data(edges, i));
  }
}

void destroy_edges(edges_list edges) {
  g_slist_free_full(edges, (void (*)(gpointer)) &destroy_edge);
}
