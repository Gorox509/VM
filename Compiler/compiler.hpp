#ifndef COMPILER_H
#define COMPILER_H

#include "../VM_commands.hpp"

#include <stdio.h>

#define EMPTY_LABEL -10

enum compiler_errors {
    COMPILER_OK = 0,
    COMPILER_CANT_SCAN_CMD,
    COMPILER_CANT_SCAN_ARG,
    COMPILER_UNKNOWN_CMD,
    COMPILER_LABEL_REDEFINITION,
    COMPILER_LABEL_OUTSIDE_LABELS_ARRAY,
    COMPILER_WRONG_LABEL,
    COMPILER_WRONG_REGISTER,
};


struct compiler {
    char *input_buf = NULL;
    int *code_buf = NULL;
    int code_len = 0;
    int code_size = 0;

    int *labels = NULL;
    int labels_size = 0;
};


int compiler_read_file_to_buffer(   FILE *fp,       struct compiler *comp,  size_t block_size);
int compiler_do_compiling(                          struct compiler *comp);
int compiler_write_buffer_to_file(  FILE *fp_out,   struct compiler *comp);
int compiler_add_label(                             struct compiler *comp,  int n_label,    int pos);
int compiler_put_label(                             struct compiler *comp,  int n_label,    int pos);
int compiler_put_register(                          struct compiler *comp,  int reg_code,   int pos);

int get_label_num(          char *arg,  int *n_label);
int get_register_code(      char *arg,  int *reg_code);
int command_code_from_str(  char *str);
int argument_from_str(      char *str);
int is_label(               char *str);
int is_register(            char *str);
int is_all_alpha(           char *str);
int is_known_cmd(           char *cmd);
int has_argument(           char *cmd);

int compiler_error(int error);


#endif
