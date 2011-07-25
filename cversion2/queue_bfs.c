#include "queue_bfs.h"

typedef struct bfs_elem bfs_elem;


struct bfs_elem{
  uint id:16;
};


struct queue_bfs{
  unsigned size;
  unsigned end;
  unsigned start;
  bfs_elem tail[0];
};


queue_bfs queue_bfs_new(uint n) {
  queue_bfs q = NULL;

  q = malloc(sizeof(struct queue_bfs) 
	     + n * sizeof(struct bfs_elem));
  q->start = 0;
  q->end = 0;
  q->size = n;

  return q;
}


void queue_bfs_push(queue_bfs q, uint n) {
  q->tail[q->end].id = n;
  q->end++;
}


void queue_bfs_destroy(queue_bfs queue){
  /*
  free(queue);
  */
}


bool queue_bfs_is_empty(queue_bfs q) {
  return (q->start == q->end);
}


uint queue_bfs_pop(queue_bfs q) {
  uint r = 0;
  r = q->tail[q->start].id;
  q->start++;
  return r;
}


bool queue_bfs_is_full(queue_bfs q) {
  return ((q->end - q->start) == q->size);
}

void queue_bfs_clear(queue_bfs q) {
  q->start = 0;
  q->end = 0;
}
