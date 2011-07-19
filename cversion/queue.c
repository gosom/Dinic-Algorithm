#include <stdlib.h>
#include <assert.h>
#include "glib.h"
#include "queue.h"
#include "utils.h"
#include "vertex.h"

queue queue_new() {
  return g_queue_new();
}

int queue_length(queue q) {
  return g_queue_get_length(q);
}

bool queue_is_empty(queue q) {
  return g_queue_is_empty(q);
}

void queue_push_head(queue q, gpointer data) {
  g_queue_push_head(q, data);
}

void queue_push_tail(queue q, gpointer data) {
  g_queue_push_tail(q, data);
}


gpointer queue_pop_head(queue q) {
  return g_queue_pop_head(q);
}

gpointer queue_pop_tail(queue q) {
  return g_queue_pop_tail(q);
}

gpointer queue_peek_head(queue q) {
  return g_queue_peek_head(q);
}

gpointer queue_peek_tail(queue q) {
  return g_queue_peek_tail(q);
}

bool queue_has_node(queue q, int node) {
  return (g_queue_find_custom(q, &node, &compare_ints) 
	  != NULL);
}


void  queue_delete_vertex(queue q, int id) {
  int i = 0, length=0;
  GList * vertex_list = NULL;
  vertex v = NULL;

  vertex_list = q->head;
  length = queue_length(q);
  
  for (i=0; i<length; i++, vertex_list=vertex_list->next) {
    v = vertex_list->data;
    if (vertex_id(v) == id) {
      destroy_vertex(v);
      g_queue_delete_link(q, vertex_list);
      break;
    }
  }
}




void queue_free(queue q, void (*free_data) (gpointer)) {
  assert(q != NULL);
  gpointer data = NULL;
  while (!queue_is_empty(q)) {
    data = queue_pop_tail(q);
    free_data(data);
  }
}
