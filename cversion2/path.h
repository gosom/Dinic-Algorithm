#ifndef _PATH_H_
#define _PATH_H_

#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

typedef struct path * path;
typedef struct snode * snode;

path path_new();
void path_destroy(path p);
void path_add_node (path p, uint i, bool b);
uint path_length(path p);
uint path_flow(path p);
uint path_nth_name(path p, uint i);
bool path_nth_balance(path p, uint i);
void path_update_flow(path p, uint flow);

#endif
