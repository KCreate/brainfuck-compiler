#ifndef BFAST_H
#define BFAST_H

// Node types for the brainfuck AST
typedef enum { bfast_instruction, bfast_loop, bfast_list } bfast_node_type_t;

// Declare all structs
typedef struct bfast_node_instruction bfast_node_instruction;
typedef struct bfast_node_loop bfast_node_loop;
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

// Represents a loop
struct bfast_node_loop {
  int start;
  int end;
  bfast_node_list children;
};

// A single node in the ast
struct bfast_node_t {

  // Doubly linked list
  bfast_node_list* list;
  bfast_node_t* next;
  bfast_node_t* prev;

  bfast_node_type_t type;
  union {
    bfast_node_instruction instruction;
    bfast_node_loop loop;
    bfast_node_loop list;
  } node;
};

// Operations that perform on a whole list
void bfast_list_append(bfast_node_list* list, bfast_node_t* entry);
void bfast_list_prepend(bfast_node_list* list, bfast_node_t* entry);

// Operations that perform on a single list node
void bfast_list_node_prepend(bfast_node_t* node, bfast_node_t* entry);
void bfast_list_node_append(bfast_node_t* node, bfast_node_t* entry);
void bfast_list_node_unlink(bfast_node_t* node);
void bfast_list_node_delete(bfast_node_t* node);

#endif
