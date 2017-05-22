#ifndef BFAST_H
#define BFAST_H

#include <stdlib.h>

// Node types for the brainfuck AST
typedef enum {
  bfast_type_instruction,
  bfast_type_list
} bfast_node_type_t;

// Declare all structs
typedef struct bfast_node_instruction bfast_node_instruction;
typedef struct bfast_node_list bfast_node_list;
typedef struct bfast_node_t bfast_node_t;

// Represents a single instruction
struct bfast_node_instruction {
  char type;
};

// Represents a list of ast nodes
struct bfast_node_list {
  int count;
  bfast_node_t* first;
  bfast_node_t* last;
};

// A single node in the ast
struct bfast_node_t {

  // Doubly linked list
  bfast_node_t* parent;
  bfast_node_t* next;
  bfast_node_t* prev;

  bfast_node_type_t type;
  union {
    bfast_node_instruction instruction;
    bfast_node_list list;
  } node;
};

// Initialisation methods
bfast_node_t* bfast_create_instruction(char type);
bfast_node_t* bfast_create_list();

// Operations that perform on a whole list
void bfast_list_append(bfast_node_t* list, bfast_node_t* entry);
void bfast_list_prepend(bfast_node_t* list, bfast_node_t* entry);

// Operations that perform on a single list node
void bfast_list_node_prepend(bfast_node_t* node, bfast_node_t* entry);
void bfast_list_node_append(bfast_node_t* node, bfast_node_t* entry);
void bfast_list_node_unlink(bfast_node_t* node);

// Methods to deallocate nodes
void bfast_destroy_node(bfast_node_t* node);
void bfast_destroy_node_rec(bfast_node_t* node);

#endif
