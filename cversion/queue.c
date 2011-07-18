#include <assert.h>
#include "glib.h"
#include "queue.h"

queue queue_new(queue q) {
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

void queue_free(queue q, void (*free_data) (gpointer)) {
  assert(q != NULL);
  gpointer data = NULL;
  while (!queue_is_empty(q)) {
    data = queue_pop_tail(q);
    free_data(data);
  }
}
