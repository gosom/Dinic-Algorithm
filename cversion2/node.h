#ifndef _NODE_H_
#define _NODE_H_

#include <stdbool.h>
#include "path.h"
#include "utils.h"

typedef struct node * node;
typedef struct nodes * nodes_list;


uint node_get_level(node n);
bool node_get_balance(node n);

node nodes_get_node(nodes_list nodes, uint i);


uint nodes_forw_get_length(nodes_list nodes, uint n);
uint nodes_back_get_length(nodes_list nodes, uint n);


void node_add_forw_neighb(node n, uint y);
void node_add_back_neighb(node n, uint x);


uint nodes_get_length(nodes_list nodes);
void nodes_set_balance(nodes_list nodes, uint n, bool balance);

void nodes_set_ancestor(nodes_list nodes, uint n, uint ancestor);
uint nodes_get_ancestor(nodes_list nodes, uint n);

void nodes_set_level(nodes_list nodes, uint x, uint level);
uint nodes_get_level(nodes_list nodes, uint x);

nodes_list nodes_new(uint size);
void nodes_destroy(nodes_list nodes);
void nodes_realloc(nodes_list nodes, uint size);

uint nodes_add(nodes_list n, uint id);
void nodes_add_neighbs(nodes_list n, uint x, uint y);

uint nodes_forw_neighb(nodes_list nodes, uint x);
uint nodes_back_neighb(nodes_list nodes, uint x);

void nodes_del_forw(nodes_list nodes, uint x);
void nodes_del_back(nodes_list nodes, uint x);

void nodes_reset_start(nodes_list nodes);
bool nodes_get_balance(nodes_list nodes, uint n);

void nodes_aux_reset(nodes_list nodes);

uint nodes_get_id(nodes_list nodes, uint n);
void nodes_set_id(nodes_list nodes, uint n, uint id);


uint nodes_forw_get_start(nodes_list n, uint x);
uint nodes_back_get_start(nodes_list n, uint x);

uint nodes_nth_forw_neighb(nodes_list n, uint x, uint i);
uint nodes_nth_back_neighb(nodes_list n, uint x, uint i);


void nodes_del_neighb(nodes_list nodes, uint x, bool bal);


void nodes_queue_bfs_add(nodes_list nodes, queue_bfs *Qq, uint i, 
						 uint level); 

void nodes_print_path(nodes_list nodes, path p, uint *flowp);


#endif
