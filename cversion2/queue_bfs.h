#ifndef _QUEUE_BFS_H_
#define _QUEUE_BFS_H_

#include <stdlib.h>
#include <stdbool.h>

typedef struct queue_bfs *queue_bfs; 
typedef unsigned int uint;

queue_bfs queue_bfs_new(uint n);
void queue_bfs_push(queue_bfs q, uint n);
void queue_bfs_destroy(queue_bfs queue);
bool queue_bfs_is_empty(queue_bfs q);
uint queue_bfs_pop(queue_bfs q);
bool queue_bfs_is_full(queue_bfs q);
void queue_bfs_clear(queue_bfs q);

#endif
