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
 * Get a unique label from the compiler
 * */
int bfc_reserve_label(bfc_compiler* compiler) {
  return compiler->block_id++;
}

/*
 * Emit a label for a given id
 * */
void bfc_emit_label(bfc_compiler* compiler, int id) {
  char output_buffer[27];
  sprintf(output_buffer, "bf_block_%d", id);
  bfc_emit(compiler, output_buffer);
}

/*
 * Emit a label for a given id plus a newline
 * */
void bfc_emitln_label(bfc_compiler* compiler, int id) {
  bfc_emit_label(compiler, id);
  EMIT("");
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
  EMIT(".def bf_ptr r0");
  EMIT(".def bf_calc1 r1b");
  EMIT(".def bf_static_one r2b");
  EMIT(".def bf_static_zero r3b");

  // Reserve space for the cells
  EMIT(".label bf_cells");
  EMIT(".org 30000");
  EMIT(".label entry_addr");

  // Default values for some registers
  EMIT("loadi bf_ptr, 0");
  EMIT("loadi bf_static_one, 1");
  EMIT("loadi bf_static_zero, 0");

  // Codegen all nodes
  bfast_node_t* cg_node = node->node.list.first;
  while (cg_node) {
    if (!bfc_compile_node(compiler, cg_node)) return false;
    cg_node = cg_node->next;
  }

  // Codegen the program exit
  EMIT("reads 1, bf_ptr");
  EMIT("push t_syscall, sys_exit");
  EMIT("syscall");

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
          EMIT("mov bf_calc1, bf_ptr");
          EMIT("add bf_calc1, bf_static_one");
          EMIT("mov bf_ptr, bf_calc1");
          break;
        }

        // Decrement the pointer
        case '<': {
          EMIT("mov bf_calc1, bf_ptr");
          EMIT("sub bf_calc1, bf_static_one");
          EMIT("mov bf_ptr, bf_calc1");
          break;
        }

        // Increment the current cell
        case '+': {
          EMIT("read bf_calc1, bf_ptr");
          EMIT("add bf_calc1, bf_static_one");
          EMIT("write bf_ptr, bf_calc1");
          break;
        }

        // Decrement the current cell
        case '-': {
          EMIT("read bf_calc1, bf_ptr");
          EMIT("sub bf_calc1, bf_static_one");
          EMIT("write bf_ptr, bf_calc1");
          break;
        }

        // Output the ASCII value of the current cell
        case '.': {
          EMIT("reads 1, bf_ptr");
          EMIT("rpush sp");
          EMIT("push t_size, 1");
          EMIT("push t_syscall, sys_write");
          EMIT("syscall");
          break;
        }

        // Read a byte from STDIN
        case ',': {
          EMIT("push t_register, bf_ptr");
          EMIT("push t_syscall, sys_read");
          EMIT("syscall");
          break;
        }

        default: {
          return false;
        }
      }

      break;
    }

    case bfast_type_list: {
      // Reserve block id's for both the entry and exit label
      int entry_label = bfc_reserve_label(compiler);
      int exit_label = bfc_reserve_label(compiler);

      // Emit the conditional jump
      EMIT("read bf_calc1, bf_ptr");
      EMIT("cmp bf_calc1, bf_static_zero");
      bfc_emit(compiler, "jz ");
      bfc_emitln_label(compiler, entry_label);

      // Emit the entry label
      bfc_emit(compiler, ".label ");
      bfc_emitln_label(compiler, entry_label);

      // Codegen the loops body
      bfast_node_t* cg_node = node->node.list.first;
      while (cg_node) {
        if (!bfc_compile_node(compiler, cg_node)) return false;
        cg_node = cg_node->next;
      }

      // Codegen loop repeat code
      EMIT("read bf_calc1, bf_ptr");
      EMIT("cmp bf_calc1, bf_static_zero");
      bfc_emit(compiler, "jz ");
      bfc_emitln_label(compiler, exit_label);
      bfc_emit(compiler, "jmp ");
      bfc_emitln_label(compiler, entry_label);
      bfc_emit(compiler, ".label ");
      bfc_emitln_label(compiler, exit_label);
      EMIT("");

      break;
    }
  }

  return true;
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
