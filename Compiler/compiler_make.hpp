#ifndef COMPILER_MAKE_H
#define COMPILER_MAKE_H

#include <stdlib.h>

struct compiler *make_compiler_struct(size_t len);

struct compiler *make_compiler_core();

int make_compiler_buffer(   struct compiler *comp, size_t len);
int make_compiler_code(     struct compiler *comp);
int make_compiler_marks(    struct compiler *comp);

int compiler_destruct(      struct compiler *comp);

#endif
