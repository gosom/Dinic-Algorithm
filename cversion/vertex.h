#ifndef VERTEX_H
#define VERTEX_H

#include <stdbool.h>

typedef struct svertex * vertex;

vertex make_vertex(int id, bool direction);
void destroy_vertex(vertex v);
int vertex_id(vertex v);
bool vertex_direction(vertex v);
void vertex_print(vertex v, void *data);

#endif
