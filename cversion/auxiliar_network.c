#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "auxiliar_network.h"
#include "edge.h"
#include "queue.h"
#include "utils.h"

void free_hash_row(gpointer data) {
  g_slist_free_full(data, (void (*) (gpointer)) destroy_vertex);
}


aux_net make_an() {
  aux_net an = NULL;

  an = g_hash_table_new(g_int_hash, g_int_equal);
  assert(an != NULL);
  return an;
}



aux_net make_auxiliar_network(edges_list edges, bool *complete) {
  aux_net an = NULL;
  int i = 0, level= 0, s=0, t=1, edges_length=0, r=0;
  edge e = NULL;
  vertex v = NULL;
  GSList * edge_data = NULL;
  bool fordward = true, backward = false;
  queue f_layer = NULL;
  queue c_layer = NULL;
  GHashTable * vertex_levels = NULL;
  int *l = NULL;
  
  
  assert(edges != NULL);
  
  *complete = false;
  /* Creating Data Structures */
  
  an = make_an();
  f_layer = queue_new();
  c_layer = queue_new();
  vertex_levels = g_hash_table_new(g_int_hash, 
				   g_int_equal);
  
  /* Initializing Data structures */

  queue_push_tail(f_layer, make_int(s));
  l = make_int(level);
  g_hash_table_insert(vertex_levels, make_int(s), l);

  while (!queue_is_empty(f_layer)) {

    /* Add new layer to auxiliar network */

    int key = *(int *)queue_pop_head(f_layer);

    edges_length = g_slist_length(edges); 
    edge_data = edges;

    for (i=0; i < edges_length; i++, edge_data=g_slist_next(edge_data)) { 

      e = edge_data->data;
      if (edge_used(e)) continue;
      r = edge_capacity(e) - edge_flow(e);
      if (key == edge_first(e) && r > 0)
	v = make_vertex(edge_last(e), fordward);
      else if (key == edge_last(e) && edge_flow(e) > 0)
	v = make_vertex(edge_first(e), backward);

      else continue;

      if (!queue_has_node(c_layer, vertex_id(v))) {
	queue_push_tail(c_layer, make_int(vertex_id(v)));
      }
      int id = vertex_id(v);
      if (g_hash_table_lookup(vertex_levels, &id) == NULL) {
	g_hash_table_insert(vertex_levels, make_int(id), make_int(level));
      }

      int *level_tmp = g_hash_table_lookup(vertex_levels, &id);
      if (*level_tmp == level) {
	an_add_edge(an, key, v);
	edge_set_used(e, true);
      }
    }
    
    if (queue_is_empty(f_layer)) {
      queue_free(f_layer, free);
      f_layer = c_layer;
      c_layer = queue_new();
      level++;
    }
    
    if (queue_has_node(f_layer, t)) {
      *complete = true;
      break;
    }

  }

  return an;
}



void destroy_auxiliar_network(aux_net an) {
  assert(an != NULL);
  g_hash_table_destroy(an);
}


void an_print(aux_net an) {
  GHashTableIter iter;
  int * key = NULL;
  GSList * value = NULL;
  int i = 0;
  vertex v = NULL;
  
  g_hash_table_iter_init(&iter, an);
  while (g_hash_table_iter_next(&iter, (gpointer) &key, (gpointer) &value)) {
    printf("key: %d\t", *key);

    for (i=0; i<g_slist_length(value); i++) {
      v = g_slist_nth_data(value, i);
      printf("id: %d\t", vertex_id(v));
    }

    /*
    g_slist_foreach(value, (void (*) (gpointer, gpointer)) 
		    &vertex_print, NULL);
    */
    printf("\n");
  }
}


void an_add_edge(aux_net an, int key, vertex v) {
  assert(an != NULL);
  assert(v != NULL);

  GSList * vertex_list = NULL;
  int * pkey = malloc(sizeof(int));
  *pkey = key;
  vertex_list = g_hash_table_lookup(an, pkey);
  vertex_list = g_slist_prepend(vertex_list, v);
  g_hash_table_insert(an, pkey, vertex_list);
}

int compare_vertex(vertex v, int * id) {
  if (vertex_id(v) == *id)
    return 0;
  return -1;
}


void an_remove_edge(aux_net an, int key, int vid) {
  assert(an != NULL);
  
  GSList * vertex_list = NULL;
  GSList * v = NULL;
  vertex_list = g_hash_table_lookup(an, make_int(key));
  assert(vertex_list != NULL);

  v = g_slist_find_custom(vertex_list, make_int(vid), 
			  (gint (*) (gconstpointer, gconstpointer)) &compare_vertex);
  assert(v != NULL);
  vertex_list = g_slist_delete_link(vertex_list, v);
  g_hash_table_insert(an, make_int(key), vertex_list);
}


vertex an_get_child(aux_net an, int father) {
  GSList * children = NULL;
  vertex v = NULL;
  
  assert(an != NULL);
  
  children = g_hash_table_lookup(an, &father);
  
  if (g_slist_length(children) > 0) {
    v = g_slist_nth_data(children, 0);
  }
  
  return v;
}

void an_remove_edges_entering(aux_net an, int key) {
  assert(an != NULL);
  
  GHashTableIter iter;
  gpointer father, children; /* key, value*/
  vertex v = NULL;

  g_hash_table_iter_init(&iter, an);
  while (g_hash_table_iter_next(&iter, &father, 
				&children)) {
    GSList * pv = g_slist_find_custom(children, 
				      &key, 
				      &compare_ints);
    if (pv == NULL) continue;
    v = pv->data;
    children = g_slist_remove(children, v);
    g_hash_table_insert(an, father, children);
  }
}

