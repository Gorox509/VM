#ifndef VM_H
#define VM_H


#include "stack/stack.hpp"
#include <stdio.h>

#ifdef VM_DEBUG
#define ON_VM_DEBUG(...) __VA_ARGS__
#define UPDATE_VM_DEBUG(proc) (proc)->error->last_called = __func__; (proc)->error->line = __LINE__; (proc)->error->filename = __FILE_NAME__;
#else
#define ON_VM_DEBUG(...)
#define UPDATE_VM_DEBUG(...)
#endif

#ifdef STACK_DEBUG
#define UPDATE_STACK_DEBUG(stk) (stk)->err_struct->last_called = __func__; (stk)->err_struct->line = __LINE__;
#else
#define UPDATE_STACK_DEBUG(...)
#endif

enum VM_errors {
    VM_OK = 0,
    VM_HALT,
    VM_NO_COMMAND_FOUND,
    VM_WRONG_COMMAND,
    VM_ZERO_PTR,
    VM_WRONG_REGISTER_CODE,
    VM_DIV_BY_ZERO,
};


struct VM_processor {
    struct stack *stack = NULL;
    int *code = NULL;
    int code_len = 0;
    int PC = 0;

    struct VM_registers *regs = NULL;

    ON_VM_DEBUG(struct VM_error *error;)
};

struct VM_registers {
    stack_elem_t regs_arr[4] = {};
    int len = 4;
};

ON_VM_DEBUG(
struct VM_error {
    const char *last_called = NULL;
    const char *filename = NULL;
    int line = 0;
    int code_line = 0;
};
)


struct VM_processor *vm_make_processor(     size_t size);
struct VM_processor *vm_make_proc_core();
int                  vm_make_proc_stack(    struct VM_processor *proc);
int                  vm_make_proc_code(     struct VM_processor *proc, size_t len);
int                  vm_make_proc_regs(     struct VM_processor *proc);
ON_VM_DEBUG(int vm_make_proc_error_struct(  struct VM_processor *proc);)

int vm_processor_destructor(                struct VM_processor *proc);
int vm_processor_dump(                      struct VM_processor *proc);

void put_divisor(FILE *fp);


int vm_do_program(              struct VM_processor *proc);
int vm_do_instruction(          struct VM_processor *proc);
int vm_handle_error(            struct VM_processor *proc,  int error);
int vm_read_file_to_buffer(     FILE *fp, char *buf, size_t block_size);
int vm_translate_buffer_to_code(struct VM_processor *proc,  char *buf,  size_t size);


int vm_do_push(     struct VM_processor *proc,  stack_elem_t value,     ssize_t *stack_err);
int vm_do_push_r(   struct VM_processor *proc,  int         reg_code,   ssize_t *stack_err);
int vm_do_out(      struct VM_processor *proc,                          ssize_t *stack_err);
int vm_do_pop_r(    struct VM_processor *proc,  int         reg_code,   ssize_t *stack_err);
int vm_do_add(      struct VM_processor *proc);
int vm_do_sub(      struct VM_processor *proc);
int vm_do_mult(     struct VM_processor *proc);
int vm_do_div(      struct VM_processor *proc);


#endif
