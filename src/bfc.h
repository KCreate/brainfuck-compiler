#ifndef BFC_H
#define BFC_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

#include "bfast.h"
#include "parser.h"

#define BFC_COMPILER_BUFFER_SIZE 8192
#define EMIT(code) bfc_emitln(compiler, code)

typedef struct {
  int block_id;
  char* buffer;
  size_t buffer_size;
  off_t buffer_pos;
} bfc_compiler;

void bfc_init(bfc_compiler* compiler);
void bfc_grow_buffer(bfc_compiler* compiler);
void bfc_emit(bfc_compiler* compiler, char* buffer);
void bfc_emitln(bfc_compiler* compiler, char* buffer);
bool bfc_compile_ast(bfc_compiler* compiler, bfast_node_t* node);
bool bfc_compile_node(bfc_compiler* compiler, bfast_node_t* node);
void bfc_dump_buffer(bfc_compiler* compiler, FILE* fp);

#endif
