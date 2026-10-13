#include "VM_proc_make_funcs.hpp"
#include "assert.h"

struct VM_processor *vm_make_processor(size_t len) {
    struct VM_processor *proc = vm_make_proc_core();
    assert(proc != NULL);

    vm_make_proc_stack(proc);
    vm_make_proc_call_stack(proc);

    vm_make_proc_code(proc, len);

    vm_make_proc_regs(proc);

    ON_VM_DEBUG(
        vm_make_proc_error_struct(proc);
    )
    UPDATE_VM_DEBUG(proc)
    return proc;
}


struct VM_processor *vm_make_proc_core() {
    struct VM_processor *proc = (struct VM_processor *) malloc(sizeof(struct VM_processor));
    if (proc == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }
    *proc = {};
    proc->PC = 0;
    proc->code_len = 0;

    return proc;
}


int vm_make_proc_stack(struct VM_processor *proc) {
    assert(proc != NULL);

    proc->stack = (struct stack *) malloc(sizeof(struct stack));
    if (proc->stack == NULL) {
        fprintf(stderr, "Error: fail to allocate memory\n");
        exit(1);
    }
    *proc->stack = {};
    stack_constructor(proc->stack, 8 ON_DEBUG(, __FILE_NAME__, __LINE__, "(no name)"));

    return VM_OK;
}


int vm_make_proc_call_stack(struct VM_processor *proc) {
    assert(proc != NULL);

    proc->call_stack = (struct stack *) malloc(sizeof(struct stack));
    if (proc->call_stack == NULL) {
        fprintf(stderr, "Error: fail to allocate memory\n");
        exit(1);
    }
    *proc->call_stack = {};
    stack_constructor(proc->call_stack, 8 ON_DEBUG(, __FILE_NAME__, __LINE__, "(no name)"));

    return VM_OK;
}


int vm_make_proc_code(struct VM_processor *proc, size_t len) {
    assert(proc != NULL);

    proc->code = (int *) calloc(len, sizeof(int));
    if (proc->code == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }

    return VM_OK;
}


int vm_make_proc_regs(struct VM_processor *proc) {
    assert(proc != NULL);

    proc->regs = (struct VM_registers *) malloc(sizeof(struct VM_registers));
    if (proc->regs == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }
    *proc->regs = {};

    return VM_OK;
}


ON_VM_DEBUG(
int vm_make_proc_error_struct(struct VM_processor *proc) {
    assert(proc != NULL);
    proc->error = (struct VM_error *) malloc(sizeof(struct VM_error));
    if (proc->error == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for error structure");
        exit(1);
    }
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

    free(proc->regs);

    ON_VM_DEBUG(
        free(proc->error);
    )

    return VM_OK;
}
