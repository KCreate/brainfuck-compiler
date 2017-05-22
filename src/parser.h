#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "bfast.h"

bfast_node_t* parse_program(char* buffer, size_t size);

#endif
