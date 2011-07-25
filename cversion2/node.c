#include <stdlib.h>
#include "node.h"

#define SOURCE_ID 0
#define TARGET_ID 1


struct node{
  uint level; 
  uint ancestor:14;
  uint balance:1; 
  uint n_neighbs_forw:14;
  uint n_neighbs_back:14;
  uint n_start_forw:14; 
  uint n_start_back:14;
  uint *neighbs_forw; 
  uint *neighbs_back; 
};

struct nodes {
  uint n_nodes;
  uint size;
  uint *ids;
  node * nodes_list;
};


uint node_get_level(node n) {
  return n->level;
}

uint node_get_balance(node n) {
  return n->balance;
}



node nodes_get_node(nodes_list nodes, uint i) {
  return nodes->nodes_list[i];
}

uint nodes_forw_get_length(nodes_list nodes, uint n) {
  return nodes->nodes_list[n]->n_neighbs_forw;
}


uint nodes_back_get_length(nodes_list nodes, uint n) {
  return nodes->nodes_list[n]->n_neighbs_back;
}


uint nodes_get_id(nodes_list nodes, uint n) {
  return nodes->ids[n];
}

void nodes_set_id(nodes_list nodes, uint n, uint id) {
  nodes->ids[n] = id;
}


uint nodes_get_length(nodes_list nodes) {
  return nodes->n_nodes;
}


void nodes_set_balance(nodes_list nodes, uint n, 
		       uint balance) {
  nodes->nodes_list[n]->balance = balance;
}

uint nodes_get_balance(nodes_list nodes, uint n) {
  return nodes->nodes_list[n]->balance;
}


void nodes_set_ancestor(nodes_list nodes, uint n, 
		       uint ancestor) {
  nodes->nodes_list[n]->ancestor = ancestor;
}


uint nodes_get_ancestor(nodes_list nodes, uint n) {
  return nodes->nodes_list[n]->ancestor;
}

void nodes_set_level(nodes_list nodes, 
		     uint x, uint level) {
  nodes->nodes_list[x]->level = level;
}

uint nodes_get_level(nodes_list nodes, uint x) {
  return nodes->nodes_list[x]->level;
}


void node_add_forw_neighb(node n, uint y) {
  uint n_forw = ++n->n_neighbs_forw;
  
  n->neighbs_forw = realloc(n->neighbs_forw,
			   n_forw * sizeof(uint));
  n->neighbs_forw[n_forw-1] = y;
}

void node_add_back_neighb(node n, uint x) {
  uint n_back = ++n->n_neighbs_back;
  
  n->neighbs_back = realloc(n->neighbs_back,
			   n_back * sizeof(uint));
  n->neighbs_back[n_back-1] = x;
}



nodes_list nodes_new(uint size) {
  uint i;
  nodes_list n;
  n = calloc(1, sizeof(struct nodes));
  n->nodes_list = calloc(size, sizeof(node));
  for (i = 0; i < size; i++) {
    n->nodes_list[i] = calloc(1, sizeof(struct node));
  }
  n->ids = calloc(size, sizeof(struct nodes));
  n->size = size;
  n->n_nodes = 2;

  return n;
}


void nodes_destroy(nodes_list n) {
  uint i;
  for (i = 0; i < n->n_nodes; i++)
    free(n->nodes_list[i]);
  free(n->nodes_list);
  free(n->ids);
  free(n);
}

void nodes_realloc(nodes_list n, uint size) {
  n->nodes_list = realloc(n->nodes_list, size);
  n->ids = realloc(n->ids, size);
  n->size = size;
}


uint nodes_add(nodes_list n, uint id) {
  uint n_nodes=n->n_nodes, pos=0, 
    s=SOURCE_ID, t=TARGET_ID;

  /* Custom settings for each type of node */

  if(id != s && id != t){
    pos = n_nodes;
    n->nodes_list[pos]->level = (uint)-1;
    n->n_nodes++;
  } else if(id != t){
    pos = 0;
    n->nodes_list[pos]->level = 0;
  } else {
    pos = 1;
    n->nodes_list[pos]->level = (uint)-1;
  }
  
  n->ids[pos] = id;

  return pos;
}


void nodes_add_neighbs(nodes_list n, uint x, uint y) {
  node_add_forw_neighb(n->nodes_list[x], y);
  node_add_back_neighb(n->nodes_list[y], x);
}


uint nodes_forw_neighb(nodes_list n, uint x) {
  uint y = 0;
  uint n_start_forw = n->nodes_list[x]->n_start_forw;
  if (n_start_forw < n->nodes_list[x]->n_neighbs_forw)
    y = n->nodes_list[x]->neighbs_forw[n_start_forw];
  return y;
}



uint nodes_back_neighb(nodes_list n, uint x) {
  uint y = 0;
  uint n_start_back = n->nodes_list[x]->n_start_back;
  if (n->nodes_list[x]->n_start_back 
      < n->nodes_list[x]->n_neighbs_back)
    y = n->nodes_list[x]->neighbs_back[n_start_back];
  return y;
}


void nodes_del_forw(nodes_list n, uint x) {
  n->nodes_list[x]->n_start_forw++;
}


void nodes_del_back(nodes_list n, uint x) {
  n->nodes_list[x]->n_start_back++;
}


uint nodes_forw_get_start(nodes_list n, uint x) {
  return n->nodes_list[x]->n_start_forw;
}


uint nodes_back_get_start(nodes_list n, uint x) {
  return n->nodes_list[x]->n_start_back;
}

uint nodes_nth_forw_neighb(nodes_list n, uint x, 
			  uint i) {
  return n->nodes_list[x]->neighbs_forw[i];
}


uint nodes_nth_back_neighb(nodes_list n, uint x, 
			  uint i) {
  return n->nodes_list[x]->neighbs_back[i];
}


void nodes_reset_start(nodes_list nodes){
  uint i;
 
  for(i = 0; i < nodes_get_length(nodes); i++){
    nodes->nodes_list[i]->n_start_forw = 0;
    nodes->nodes_list[i]->n_start_back = 0;
  }
}

void nodes_aux_reset(nodes_list nodes){
  uint i;

  for(i = 1; i < nodes_get_length(nodes); i++)
    nodes_set_level(nodes, i, (uint)-1);
}
