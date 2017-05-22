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

  // Emit empty line for visual purposes
  EMIT("");

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

          // Emit empty line for visual purposes
          EMIT("");
          break;
        }

        // Decrement the pointer
        case '<': {
          EMIT("mov bf_calc1, bf_ptr");
          EMIT("sub bf_calc1, bf_static_one");
          EMIT("mov bf_ptr, bf_calc1");

          // Emit empty line for visual purposes
          EMIT("");
          break;
        }

        // Increment the current cell
        case '+': {
          EMIT("read bf_calc1, bf_ptr");
          EMIT("add bf_calc1, bf_static_one");
          EMIT("write bf_ptr, bf_calc1");

          // Emit empty line for visual purposes
          EMIT("");
          break;
        }

        // Decrement the current cell
        case '-': {
          EMIT("read bf_calc1, bf_ptr");
          EMIT("sub bf_calc1, bf_static_one");
          EMIT("write bf_ptr, bf_calc1");

          // Emit empty line for visual purposes
          EMIT("");
          break;
        }

        // Output the ASCII value of the current cell
        case '.': {
          EMIT("reads 1, bf_ptr");
          EMIT("rpush sp");
          EMIT("push t_size, 1");
          EMIT("push t_syscall, sys_write");
          EMIT("syscall");

          // Emit empty line for visual purposes
          EMIT("");

          break;
        }

        // Read a byte from STDIN
        // Unimplemented right now
        case ',': {
          EMIT("Emitting stdin-read");
          break;
        }

        default: {
          EMIT("Emitting unknown instruction");
          break;
        }
      }

      break;
    }

    case bfast_type_list: {

      // Reserve block id's for both the entry and exit label
      char entry_label_buffer[27], exit_label_buffer[27];
      sprintf(entry_label_buffer, "bf_block_%d", compiler->block_id++);
      sprintf(exit_label_buffer, "bf_block_%d", compiler->block_id++);

      // Emit the conditional jump
      EMIT("read bf_calc1, bf_ptr");
      EMIT("cmp bf_calc1, bf_static_zero");
      bfc_emit(compiler, "jz ");
      EMIT(exit_label_buffer);
      EMIT("");

      // Emit the entry label label
      bfc_emit(compiler, ".label ");
      EMIT(entry_label_buffer);

      // Codegen the loops body
      bfast_node_t* cg_node = node->node.list.first;
      while (cg_node) {
        if (!bfc_compile_node(compiler, cg_node)) return false;
        cg_node = cg_node->next;
      }

      // Codegen loop repeat code
      //
      // check to flags
      // jz block_end
      // jmp block_start
      // .label block_end
      EMIT("read bf_calc1, bf_ptr");
      EMIT("cmp bf_calc1, bf_static_zero");
      bfc_emit(compiler, "jz ");
      EMIT(exit_label_buffer);
      bfc_emit(compiler, "jmp ");
      EMIT(entry_label_buffer);
      bfc_emit(compiler, ".label ");
      EMIT(exit_label_buffer);
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
