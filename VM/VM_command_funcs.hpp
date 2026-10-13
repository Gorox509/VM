#ifndef VM_CMD_FUNCS_H
#define VM_CMD_FUNCS_H

#include "../stack/stack.hpp"

int vm_do_push(     struct VM_processor *proc,  stack_elem_t value,     ssize_t *stack_err);
int vm_do_push_r(   struct VM_processor *proc,  int         reg_code,   ssize_t *stack_err);
int vm_do_out(      struct VM_processor *proc,                          ssize_t *stack_err);
int vm_do_pop_r(    struct VM_processor *proc,  int         reg_code,   ssize_t *stack_err);
int vm_do_add(      struct VM_processor *proc);
int vm_do_sub(      struct VM_processor *proc);
int vm_do_mult(     struct VM_processor *proc);
int vm_do_div(      struct VM_processor *proc);

int vm_do_jump_if(  struct VM_processor *proc, int arg,
                    int (*compare_for_jump)(stack_elem_t *, stack_elem_t *));

int vm_do_in(       struct VM_processor *proc);
int vm_do_call(     struct VM_processor *proc, int value);
int vm_do_ret(      struct VM_processor *proc);

#endif
