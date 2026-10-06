#ifndef VM_H
#define VM_H


#include "stack/stack.hpp"
#include <stdio.h>


enum VM_errors {
    VM_OK = 0,
    VM_NO_COMMAND_FOUND,
};


int vm_do_program(          struct stack *stk,  char *buf);
int vm_do_instruction(      struct stack *stk,  char *buf,      size_t *PC);
int vm_handle_error(        int error);
int vm_read_file_to_buffer( FILE *fp,           char *buffer,   size_t block_size);

void vm_do_push(    struct stack *stk,  stack_elem_t value, ssize_t *err);
void vm_do_out(     struct stack *stk);
void vm_do_add(     struct stack *stk);
void vm_do_sub(     struct stack *stk);
void vm_do_mult(    struct stack *stk);
void vm_do_div(     struct stack *stk);


#endif
