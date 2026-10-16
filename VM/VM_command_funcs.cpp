#include "VM_command_funcs.hpp"
#include <assert.h>
#include "VM.hpp"
#include "../stack/stack.hpp"

int vm_do_PUSH(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);

    ssize_t err = STACK_OK;
    int cmd = proc->code[proc->PC];

    int *arg_p = proc->code + proc->PC + 1;
    int res = 0;
    if (cmd & 0x20) {
        res += *arg_p;
        arg_p++;
    }
    if (cmd & 0x40) {
        res += proc->regs->regs_arr[*arg_p];
        arg_p++;
    }
    /*if (cmd & 0x80) {
        res += proc->RAM->arr[*arg_p]; TODO: uncomment when RAM
        arg_p++;
    }*/

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    stack_push(proc->stack, res, &err); // TODO: to bits

    return VM_OK;
}


int vm_do_OUT(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    stack_elem_t value = 0;

    ssize_t err = STACK_OK;

    stack_pop(proc->stack, &value, &err);

    fprintf(stdout, "%" ELEM_SPEC "\n", value);

    return VM_OK;
}


int vm_do_POP(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->regs != NULL);

    int reg_code = proc->code[++proc->PC];
    ssize_t err = STACK_OK;

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    if (REGS < reg_code - 1 || reg_code <= 0) {
        return VM_WRONG_REGISTER_CODE;
    }

    stack_pop(proc->stack, &proc->regs->regs_arr[reg_code - 1], &err);

    return VM_OK;
}


int vm_do_ADD(struct VM_processor *proc) {
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


int vm_do_SUB(struct VM_processor *proc) {
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


int vm_do_MUL(struct VM_processor *proc) {
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


int vm_do_DIV(struct VM_processor *proc) {
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


int vm_do_JMP(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int arg = proc->code[++proc->PC];

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    proc->PC = arg;

    return VM_OK;
}


int vm_do_JA(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int arg = proc->code[++proc->PC];

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (val1 > val2) proc->PC = arg;

    return VM_OK;
}


int vm_do_JAE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int arg = proc->code[++proc->PC];

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (val1 >= val2) proc->PC = arg;

    return VM_OK;
}


int vm_do_JB(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int arg = proc->code[++proc->PC];

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (val1 < val2) proc->PC = arg;

    return VM_OK;
}


int vm_do_JBE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int arg = proc->code[++proc->PC];

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (val1 <= val2) proc->PC = arg;

    return VM_OK;
}


int vm_do_JE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int arg = proc->code[++proc->PC];

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (val1 == val2) proc->PC = arg;

    return VM_OK;
}


int vm_do_JNE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int arg = proc->code[++proc->PC];

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (val1 != val2) proc->PC = arg;

    return VM_OK;
}


int vm_do_IN(struct VM_processor *proc) {
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


int vm_do_CALL(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->call_stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    UPDATE_STACK_DEBUG(proc->call_stack)

    int value = proc->code[++proc->PC];
    ssize_t call_err = STACK_OK;

    stack_push(proc->call_stack, proc->PC, &call_err);

    proc->PC = value;

    return VM_OK;
}


int vm_do_RET(struct VM_processor *proc) {
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


int vm_do_HLT(struct VM_processor *proc) {
    assert(proc != NULL);

    return VM_HALT;
}
