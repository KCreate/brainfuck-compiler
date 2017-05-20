#include "bfast.h"

/*
 * Append a node to the end of the list
 * */
void bfast_list_append(bfast_node_list* list, bfast_node_t* entry) {
  if (!list || !entry) { return; }

  // Check if the list has any nodes
  if (list->count == 0) {
    list->first = entry;
    list->last = entry;
    return;
  }

  // Append the entry to the list
  entry->prev = list->last;
  list->last->next = entry;
  list->last = entry;
  list->count++;
  entry->list = list;
}

/*
 * Prepend a node to the beginning of the list
 * */
void bfast_list_prepend(bfast_node_list* list, bfast_node_t* entry) {
  if (!list || !entry) { return; }

  // Check if the list has any nodes
  if (list->count == 0) {
    list->first = entry;
    list->last = entry;
    return;
  }

  // Prepend the entry to the list
  entry->next = list->first;
  list->first->prev = entry;
  list->first = entry;
  list->count++;
  entry->list = list;
}

/*
 * Prepend an entry to another entry
 * */
void bfast_list_node_prepend(bfast_node_t* node, bfast_node_t* entry) {
  if (!node || !entry) { return; }


}

void bfast_list_node_append(bfast_node_t* node, bfast_node_t* entry);
void bfast_list_node_unlink(bfast_node_t* node);
void bfast_list_node_delete(bfast_node_t* node);
