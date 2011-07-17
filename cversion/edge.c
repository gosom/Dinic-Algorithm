#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "edge.h"


struct sedge {
  int first;
  int last;
  int capacity;
  int flow;
  bool used;
};


edge make_edge(int first, int last, int capacity, 
	       int flow, bool used) {
  edge e = malloc(sizeof(struct sedge));
  e->first = first;
  e->last = last;
  e->capacity = capacity;
  e->flow = flow;
  e->used = used;

  return e;
}

void destroy_edge(edge e) {
  free(e);
  e = NULL;
}

int edge_first(edge e) {
  return e->first;
}

int edge_last(edge e) {
  return e->last;
}

int edge_capacity(edge e) {
  return e->capacity;
}

int edge_flow(edge e) {
  return e->flow;
}

void update_flow(edge e, int flow) {
  e->flow = flow;
}

bool edge_used(edge e) {
  return e->used;
}

void print_edge(edge e) {
  printf("%d %d %d %d %s\n", 
	 edge_first(e), 
	 edge_last(e),
	 edge_capacity(e),
	 edge_flow(e),
	 edge_used(e)?"True":"False");
}


void edge_set_used(edge e, bool used) {
  assert(e != NULL);
  e->used = used;
}
