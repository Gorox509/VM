#ifndef COMPILER_H
#define COMPILER_H

#include "VM_commands.hpp"

#include <stdio.h>


enum compiler_errors {
    COMPILER_OK = 0,
};


int compiler_read_file_to_buffer(FILE *fp, char *buffer, size_t block_size);
int compiler_do_compiling_from_buffer_to_new_buffer(char *buf, char *buf_new);


#endif
