#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "vertex.h"

struct svertex {
  int id;
  bool direction;
};


vertex make_vertex(int id, bool direction) {
  vertex v = NULL;

  v = calloc(1, sizeof(struct svertex));
  v->id = id;
  v->direction = direction;
  
  return v;
}


void destroy_vertex(vertex v) {
  assert(v != NULL);
  free(v);
  v = NULL;
}


int vertex_id(vertex v) {
  assert(v != NULL);
  return v->id;
}

bool vertex_direction(vertex v) {
  assert(v != NULL);
  return v->direction;
}

void vertex_print(vertex v, void * data) {
  assert(v != NULL);
  printf("ID: %d Dir: %s\t", v->id, (v->direction)?"F":"B");
}
