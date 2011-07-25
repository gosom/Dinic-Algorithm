#ifndef _PATH_H_
#define _PATH_H_

#include <stdlib.h>

typedef struct path * path;
typedef struct snode * snode;

typedef unsigned int uint;
typedef unsigned short int usint;


path path_new();
void path_destroy(path p);
void path_add_node (path p, uint i, uint b);
uint path_length(path p);
uint path_flow(path p);
uint path_nth_name(path p, uint i);
uint path_nth_balance(path p, uint i);
void path_update_flow(path p, uint flow);

#endif
