#include <assert.h>
#include "path.h"


struct snode{
	uint balance:1;
	uint name:15;
};

struct path{
	uint flow:31;
	uint n_nodes:17;
	snode *nodes;
};


path path_new() {
	path path = NULL;

	path = malloc(sizeof(struct path));
	path->n_nodes=0;
	path->nodes = NULL;

	return path;
}

void path_destroy(path p){
	uint i;
	assert(p != NULL);
	for (i = 0; i < p->n_nodes; i++)
		free(p->nodes[i]);
	free(p->nodes);
	free(p);
	p = NULL;
}

void path_add_node (path p, uint i, uint b) {
	p->nodes = realloc(p->nodes, 
					   (p->n_nodes+1)
					   * sizeof(snode));
	p->nodes[p->n_nodes] = malloc(sizeof(struct snode));
	p->nodes[p->n_nodes]->balance = b;
	p->nodes[p->n_nodes]->name = i;
	p->n_nodes++;
}

uint path_length(path p) {
	return p->n_nodes;
}

uint path_flow(path p) {
	return p->flow;
}

void path_update_flow(path p, uint flow) {
	p->flow = flow;
}

uint path_nth_name(path p, uint i) {
	return p->nodes[i]->name;
}

uint path_nth_balance(path p, uint i) {
	return p->nodes[i]->balance;
}
