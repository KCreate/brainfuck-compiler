#include "bfc.h"

/*
 * Initialize a bfc compiler struct
 * */
void bfc_init(bfc_compiler* compiler) {
  compiler->block_id = 0;
  compiler->buffer = malloc(BFC_COMPILER_BUFFER_SIZE);
  compiler->buffer_size = BFC_COMPILER_BUFFER_SIZE;
  compiler->buffer_pos = 0;
}

/*
 * Grow the buffer of the compiler to double it's original size
 * */
void bfc_grow_buffer(bfc_compiler* compiler) {
  char* new_buffer = calloc(sizeof(char), compiler->buffer_size * 2);
  memmove(new_buffer, compiler->buffer, compiler->buffer_size);
  compiler->buffer = new_buffer;
  compiler->buffer_size *= 2;
}

/*
 * Emit a string
 * */
void bfc_emit(bfc_compiler* compiler, char* buffer) {
  size_t buffer_length = strlen(buffer);

  // Grow compiler buffer if we ran out of space
  if (compiler->buffer_pos + buffer_length > compiler->buffer_size) {
    bfc_grow_buffer(compiler);
  }

  // Write into the buffer
  char* target = compiler->buffer + compiler->buffer_pos;
  strcpy(target, buffer);
  compiler->buffer_pos += buffer_length;
}

/*
 * Emit a string plus a newline
 * */
void bfc_emitln(bfc_compiler* compiler, char* buffer) {
  bfc_emit(compiler, buffer);
  bfc_emit(compiler, "\n");
}

/*
 * Dump the compilers internal buffer into a file
 * */
void bfc_dump_buffer(bfc_compiler* compiler, FILE* fp) {
  fwrite(compiler->buffer, compiler->buffer_pos, 1, fp);
}

/*
 * Codegen a top-level node
 * */
bool bfc_compile_ast(bfc_compiler* compiler, bfast_node_t* node) {

  // Check node type
  if (node->type != bfast_type_list) {
    return false;
  }

  /*
   * Next we emit the prologue to the brainfuck program
   * We reserve space for the cells and create definitions for
   * some commonly-used registers
   * */

  // Create definitions for commonly-used registers
  bfc_emitln(compiler, ".def bf_ptr r0");
  bfc_emitln(compiler, ".def bf_calc1 r1b");
  bfc_emitln(compiler, ".def bf_calc2 r2b");

  // Reserve space for the cells
  bfc_emitln(compiler, ".label bf_cells");
  bfc_emitln(compiler, ".org 30000");
  bfc_emitln(compiler, ".label entry_addr");

  // Default values for some registers
  bfc_emitln(compiler, "loadi bf_ptr, 0");
  bfc_emitln(compiler, "loadi bf_calc2, 1");

  // Emit empty line for visual purposes
  bfc_emitln(compiler, "");

  // Codegen all nodes
  bfast_node_t* cg_node = node->node.list.first;
  while (cg_node) {
    bfc_compile_node(compiler, cg_node);
    cg_node = cg_node->next;
  }

  // Codegen the program exit
  bfc_emitln(compiler, "reads 1, bf_ptr");
  bfc_emitln(compiler, "push t_syscall, sys_exit");
  bfc_emitln(compiler, "syscall");

  return true;
}

/*
 * Codegen a single node
 * */
bool bfc_compile_node(bfc_compiler* compiler, bfast_node_t* node) {
  switch (node->type) {
    case bfast_type_instruction: {

      switch (node->node.instruction.type) {

        // Increment the pointer
        case '>': {
          bfc_emitln(compiler, "mov bf_calc1, bf_ptr");
          bfc_emitln(compiler, "add bf_calc1, bf_calc2");
          bfc_emitln(compiler, "mov bf_ptr, bf_calc1");

          // Emit empty line for visual purposes
          bfc_emitln(compiler, "");
          break;
        }

        // Decrement the pointer
        case '<': {
          bfc_emitln(compiler, "mov bf_calc1, bf_ptr");
          bfc_emitln(compiler, "sub bf_calc1, bf_calc2");
          bfc_emitln(compiler, "mov bf_ptr, bf_calc1");

          // Emit empty line for visual purposes
          bfc_emitln(compiler, "");
          break;
        }

        // Increment the current cell
        case '+': {
          bfc_emitln(compiler, "read bf_calc1, bf_ptr");
          bfc_emitln(compiler, "add bf_calc1, bf_calc2");
          bfc_emitln(compiler, "write bf_ptr, bf_calc1");

          // Emit empty line for visual purposes
          bfc_emitln(compiler, "");
          break;
        }

        // Decrement the current cell
        case '-': {
          bfc_emitln(compiler, "read bf_calc1, bf_ptr");
          bfc_emitln(compiler, "sub bf_calc1, bf_calc2");
          bfc_emitln(compiler, "write bf_ptr, bf_calc1");

          // Emit empty line for visual purposes
          bfc_emitln(compiler, "");
          break;
        }

        // Output the ASCII value of the current cell
        case '.': {
          bfc_emitln(compiler, "reads 1, bf_ptr");
          bfc_emitln(compiler, "rpush sp");
          bfc_emitln(compiler, "push t_size, 1");
          bfc_emitln(compiler, "push t_syscall, sys_write");
          bfc_emitln(compiler, "syscall");

          // Emit empty line for visual purposes
          bfc_emitln(compiler, "");

          break;
        }

        default: {
          bfc_emitln(compiler, "emitting unknown instruction");
          break;
        }
      }

      break;
    }

    case bfast_type_list: {
      bfc_emitln(compiler, "emitting loop");
      break;
    }
  }

  return false;
}

int main(int argc, char** argv) {

  // Check argument count
  if (argc <= 1) {
    fprintf(stderr, "Missing filename\n");
    return 1;
  }

  // Open the file
  FILE* fp;
  if (!(fp = fopen(argv[1], "r"))) {
    fprintf(stderr, "Could not open file: %s\n", argv[1]);
    return 1;
  }

  struct stat inputStat;
  if (fstat(fileno(fp), &inputStat) < 0) {
    fprintf(stderr, "Could not stat file %s\n", argv[1]);
    return 1;
  }

  char* buffer = malloc(inputStat.st_size);
  fread(buffer, inputStat.st_size, 1, fp);

  // Parse the input string
  bfast_node_t* instructions = parse_program(buffer, inputStat.st_size);

  // Create a new compiler
  bfc_compiler compiler;
  bfc_init(&compiler);

  // Generate code and dump to stdout
  if (!bfc_compile_ast(&compiler, instructions)) {
    fprintf(stderr, "Could not compile source file %s\n", argv[1]);
  } else {
    bfc_dump_buffer(&compiler, stdout);
  }

  bfast_destroy_node_rec(instructions);
  free(buffer);
  fclose(fp);

  return 0;
}
