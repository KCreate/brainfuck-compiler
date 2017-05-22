#include "parser.h"

/*
 * Parse a brainfuck program
 * */
bfast_node_t* parse_program(char* buffer, size_t size) {
  bfast_node_t* top = bfast_create_list();

  for (int i = 0; i < size; i++) {
    switch (buffer[i]) {

      // Parse basic single char instructions
      case '+':
      case '-':
      case '<':
      case '>':
      case ',':
      case '.': {
        bfast_list_append(top, bfast_create_instruction(buffer[i]));
        break;
      }

      // Parse a loop
      case '[': {
        bfast_node_t* list = bfast_create_list();
        bfast_list_append(top, list);
        top = list;
        break;
      }

      // Close a loop
      case ']': {

        // Check if we're at the top
        if (top->parent == NULL) {
          fprintf(stderr, "Unexpected ']', missing '[' at offset %d\n", i);
          exit(1);
        }

        // Jump up one level
        top = top->parent;
        break;
      }

      // Ignoreable characters
      case ' ':
      case '\t':
      case '\n':
      case '\r': {
        break;
      }
    }
  }

  return top;
}

