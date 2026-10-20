#include "VM_proc_make_funcs.hpp"
#include "assert.h"

#define ALLOC_FAILED {                                                          \
                        fprintf(stderr, "Error: fail to allocate memory\n");    \
                        exit(1);                                                \
                     }


struct VM_processor *vm_make_processor(size_t len) {
    struct VM_processor *proc = vm_make_proc_core();
    assert(proc != NULL);

    vm_make_proc_stack(proc);
    vm_make_proc_call_stack(proc);

    vm_make_proc_code(proc, len);

    vm_make_proc_regs(proc);

    vm_make_proc_RAM(proc);

    ON_VM_DEBUG(
        vm_make_proc_error_struct(proc);
    )
    UPDATE_VM_DEBUG(proc)
    return proc;
}


struct VM_processor *vm_make_proc_core() {
    struct VM_processor *proc = (struct VM_processor *) malloc(sizeof(struct VM_processor));
    if (proc == NULL)
        ALLOC_FAILED

    *proc = {};
    proc->PC = 0;
    proc->code_len = 0;

    return proc;
}


int vm_make_proc_stack(struct VM_processor *proc) {
    assert(proc != NULL);

    proc->stack = (struct stack *) malloc(sizeof(struct stack));
    if (proc->stack == NULL)
        ALLOC_FAILED

    *proc->stack = {};
    stack_constructor(proc->stack, 8 ON_DEBUG(, __FILE_NAME__, __LINE__, "(no name)"));

    return VM_OK;
}


int vm_make_proc_call_stack(struct VM_processor *proc) {
    assert(proc != NULL);

    proc->call_stack = (struct stack *) malloc(sizeof(struct stack));
    if (proc->call_stack == NULL)
        ALLOC_FAILED

    *proc->call_stack = {};
    stack_constructor(proc->call_stack, 8 ON_DEBUG(, __FILE_NAME__, __LINE__, "(no name)"));

    return VM_OK;
}


int vm_make_proc_code(struct VM_processor *proc, size_t len) {
    assert(proc != NULL);

    proc->code = (char *) calloc(len, sizeof(char));
    if (proc->code == NULL)
        ALLOC_FAILED

    return VM_OK;
}


int vm_make_proc_regs(struct VM_processor *proc) {
    assert(proc != NULL);

    proc->regs = (struct VM_registers *) malloc(sizeof(struct VM_registers));
    if (proc->regs == NULL)
        ALLOC_FAILED

    *proc->regs = {};

    return VM_OK;
}


int vm_make_proc_RAM(struct VM_processor *proc) {
    assert(proc != NULL);

    proc->RAM_len = 1024; // TODO: no hard-code
    proc->RAM = (stack_elem_t *) calloc((size_t) proc->RAM_len, sizeof(stack_elem_t));
    if (proc->RAM == NULL)
        ALLOC_FAILED

    return VM_OK;
}


ON_VM_DEBUG(
int vm_make_proc_error_struct(struct VM_processor *proc) {
    assert(proc != NULL);
    proc->error = (struct VM_error *) malloc(sizeof(struct VM_error));
    if (proc->error == NULL)
        ALLOC_FAILED

    *proc->error = {};
    proc->error->last_called = __func__;
    proc->error->filename = __FILE_NAME__;

    return VM_OK;
}
)


int vm_processor_destructor(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->code != NULL);
    assert(proc->regs != NULL);

    stack_destructor(proc->stack);
    free(proc->stack);
    stack_destructor(proc->call_stack);
    free(proc->call_stack);
    free(proc->code);
    free(proc->RAM);

    free(proc->regs);

    ON_VM_DEBUG(
        free(proc->error);
    )

    return VM_OK;
}
