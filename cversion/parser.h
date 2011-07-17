#ifndef PARSER_H
#define PARSER_H

#include "glib.h"

typedef GSList * edges_list;

edges_list read_edges();
void print_edges(edges_list);
void open_file(const char * filename);

#endif
