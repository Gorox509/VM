#ifndef VM_H
#define VM_H


#include "../stack/stack.hpp"
#include <stdio.h>

#define REGS 4

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
    VM_WRONG_PC,
    VM_RAM_OUT_OF_BOUNDS,
    VM_POP_CONST,
    VM_STACK_ERROR,
    VM_CALL_STACK_ERROR,
};


struct OpCode {
    int op      : 5;
    int konst   : 1;
    int reg     : 1;
    int mem     : 1;
};


struct VM_processor {
    struct stack *stack = NULL;
    char *code = NULL;
    int code_len = 0;
    int PC = 0;

    stack_elem_t *RAM = NULL;
    int RAM_len = 0;

    bool text_mode = 0;

    struct VM_registers *regs = NULL;

    struct stack *call_stack = NULL;

    ON_VM_DEBUG(struct VM_error *error;)
};

struct VM_registers {
    stack_elem_t regs_arr[REGS] = {};
};

ON_VM_DEBUG(
struct VM_error {
    const char *last_called = NULL;
    const char *filename = NULL;
    int line = 0;
    int code_line = 0;
};
)

int vm_run(int argc, char *argv[]);

int vm_processor_dump(          struct VM_processor *proc);
void vm_mem_dump(               struct VM_processor *proc);

int vm_do_program(              struct VM_processor *proc);
int vm_do_instruction(          struct VM_processor *proc);
int vm_handle_error(            struct VM_processor *proc,  int error);

#endif
