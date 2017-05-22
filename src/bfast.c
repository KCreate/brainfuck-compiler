#include "bfast.h"

/*
 * Create a new instruction node
 * */
bfast_node_t* bfast_create_instruction(char type) {
  bfast_node_t* node = malloc(sizeof(bfast_node_t));

  node->next = NULL;
  node->prev = NULL;
  node->type = bfast_type_instruction;
  node->node.instruction.type = type;

  return node;
}

/*
 * Create a new list node
 * */
bfast_node_t* bfast_create_list() {
  bfast_node_t* node = malloc(sizeof(bfast_node_t));

  node->next = NULL;
  node->prev = NULL;
  node->type = bfast_type_list;
  node->node.list.count = 0;
  node->node.list.first = NULL;
  node->node.list.last = NULL;

  return node;
}

/*
 * Append a node to the end of the list
 * */
void bfast_list_append(bfast_node_t* list, bfast_node_t* entry) {
  if (!list || !entry) { return; }

  // Check if the list has any nodes
  if (list->node.list.count == 0) {
    list->node.list.first = entry;
    list->node.list.last = entry;
    list->node.list.count++;
    return;
  }

  // Append the entry to the list
  entry->prev = list->node.list.last;
  list->node.list.last->next = entry;
  list->node.list.last = entry;
  list->node.list.count++;
  entry->parent = list;
}

/*
 * Prepend a node to the beginning of the list
 * */
void bfast_list_prepend(bfast_node_t* list, bfast_node_t* entry) {
  if (!list || !entry) { return; }

  // Check if the list has any nodes
  if (list->node.list.count == 0) {
    list->node.list.first = entry;
    list->node.list.last = entry;
    list->node.list.count++;
    return;
  }

  // Prepend the entry to the list
  entry->next = list->node.list.first;
  list->node.list.first->prev = entry;
  list->node.list.first = entry;
  list->node.list.count++;
  entry->parent = list;
}

/*
 * Prepend an entry to another entry
 *
 * prev <-> node
 * prev <-> entry <-> node
 * */
void bfast_list_node_prepend(bfast_node_t* node, bfast_node_t* entry) {
  if (!node || !entry) { return; }

  // Check if the node has any previous nodes
  if (!node->prev) {
    node->prev = entry;
    entry->next = node;
    entry->parent = node->parent;
    node->parent->node.list.count++;
    node->parent->node.list.first = entry;
    return;
  }

  // Link the entry
  bfast_node_t* prev = node->prev;
  prev->next = entry;
  node->prev = entry;
  entry->prev = prev;
  entry->next = node;
  entry->parent = node->parent;
  node->parent->node.list.count++;
}

/*
 * Append an entry to another entry
 *
 * node <-> next
 * node <-> entry <-> next
 * */
void bfast_list_node_append(bfast_node_t* node, bfast_node_t* entry) {
  if (!node || !entry) { return; }

  // Check if the node has a next node
  if (!node->next) {
    node->next = entry;
    entry->prev = node;
    entry->parent = node->parent;
    node->parent->node.list.count++;
    node->parent->node.list.last = entry;
    return;
  }

  // Link the entry
  bfast_node_t* next = node->next;
  next->prev = entry;
  node->next = entry;
  entry->prev = node;
  entry->next = next;
  entry->parent = node->parent;
  node->parent->node.list.count++;
}


/*
 * Unlink a node
 * */
void bfast_list_node_unlink(bfast_node_t* node) {
  if (!node) {
    return;
  }

  // Close the gap between the next and prev node
  if (node->prev) { node->prev->next = node->next; }
  if (node->next) { node->next->prev = node->prev; }

  // Update the parent list
  if (node->parent) node->parent->node.list.count--;
  if (node->parent->node.list.first == node) node->parent->node.list.first = node->next;
  if (node->parent->node.list.last == node) node->parent->node.list.last = node->prev;
}

/*
 * Destroy a single node
 * */
void bfast_destroy_node(bfast_node_t* node) {
  free(node);
}

/*
 * Destroy a node and all it's subnodes
 * */
void bfast_destroy_node_rec(bfast_node_t* node) {

  // If the node is a list, destory all children too
  if (node->type == bfast_type_list) {
    bfast_node_t* item = node->node.list.first;

    while (item) {
      bfast_node_t* backup_next = item->next;
      bfast_destroy_node_rec(item);
      item = backup_next;
    }
  }

  bfast_destroy_node(node);
}
