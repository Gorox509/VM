#include "VM_command_funcs.hpp"
#include <assert.h>
#include "VM.hpp"

int vm_do_push(struct VM_processor *proc, stack_elem_t value, ssize_t *err) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(err != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    stack_push(proc->stack, value, err);

    return VM_OK;
}


int vm_do_push_r(struct VM_processor *proc, int reg_code, ssize_t *err) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->regs !=  NULL);
    assert(err != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    if (proc->regs->len < reg_code - 1 || reg_code <= 0) {
        return VM_WRONG_REGISTER_CODE;
    }

    stack_elem_t reg_val = proc->regs->regs_arr[reg_code - 1];

    stack_push(proc->stack, reg_val, err);

    return VM_OK;
}


int vm_do_out(struct VM_processor *proc, ssize_t *stack_err) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    stack_elem_t value = 0;

    stack_pop(proc->stack, &value, stack_err);

    fprintf(stdout, "%" ELEM_SPEC "\n", value);

    return VM_OK;
}


int vm_do_pop_r(struct VM_processor *proc, int reg_code, ssize_t *stack_err) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->regs != NULL);

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    if (proc->regs->len < reg_code - 1 || reg_code <= 0) {
        return VM_WRONG_REGISTER_CODE;
    }

    stack_pop(proc->stack, &proc->regs->regs_arr[reg_code - 1], stack_err);

    return VM_OK;
}


int vm_do_add(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    stack_push(proc->stack, val1 + val2, &err);

    return VM_OK;
}


int vm_do_sub(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    stack_push(proc->stack, val1 - val2, &err);

    return VM_OK;
}


int vm_do_mult(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    stack_push(proc->stack, val1 * val2, &err);

    return VM_OK;
}


int vm_do_div(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (val2 == 0) {
        return VM_DIV_BY_ZERO;
    }

    stack_push(proc->stack, val1 / val2, &err);

    return VM_OK;
}


int vm_do_jump_if(struct VM_processor *proc, int arg, int (*compare_for_jump)(stack_elem_t *, stack_elem_t *)) {
    assert(proc != NULL);
    assert(compare_for_jump != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (compare_for_jump(&val1, &val2))
        proc->PC = arg;

    return VM_OK;
}


int vm_do_in(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    stack_elem_t val = 0;
    ssize_t err = STACK_OK;

    scanf("%" ELEM_SPEC, &val);

    stack_push(proc->stack, val, &err);

    return VM_OK;
}


int vm_do_call(struct VM_processor *proc, int value) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->call_stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    UPDATE_STACK_DEBUG(proc->call_stack)

    ssize_t call_err = STACK_OK;

    stack_push(proc->call_stack, proc->PC, &call_err);

    proc->PC = value;

    return VM_OK;
}


int vm_do_ret(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->call_stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    UPDATE_STACK_DEBUG(proc->call_stack)

    ssize_t call_err = STACK_OK;

    stack_pop(proc->call_stack, &proc->PC, &call_err);

    return VM_OK;
}
