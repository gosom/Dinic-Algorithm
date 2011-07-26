#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "network.h"
#include "utils.h"

typedef struct output * output;



void out_set_net(output out, Net net);
void out_path_destroy(output out);

void print_paths(output out,int flags, int na);

void out_add_path(path S, output out);

uint out_mincut_capacity(output out); 

void print_output(output out, int flags, int time[], int runs); 


output out_new(void);

void out_set_flow(output out, uint flow);
uint out_get_flow(output out);

#endif 
