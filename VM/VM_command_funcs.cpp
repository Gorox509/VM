#include "VM_command_funcs.hpp"
#include <assert.h>
#include <memory.h>
#include "VM.hpp"
#include "../stack/stack.hpp"

#define GET_ARG(ptr)    stack_elem_t arg = 0;                                                   \
                        stack_elem_t *arg_ptr = (stack_elem_t *) malloc(sizeof(stack_elem_t));  \
                        memcpy(arg_ptr, (ptr), sizeof(stack_elem_t));                           \
                        arg = *arg_ptr;                                                         \
                        free(arg_ptr);



int vm_do_PUSH(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    ssize_t err = STACK_OK;
    char cmd = proc->code[proc->PC];

    stack_elem_t *arg_p = (stack_elem_t *) (proc->code + proc->PC + 1);
    int res = 0;
    if (cmd & 0x20) {
        GET_ARG(arg_p)
        res += arg;
        arg_p++;
        proc->PC += (int) sizeof(stack_elem_t);
    }
    if (cmd & 0x40) {
        GET_ARG(arg_p)
        if (arg < 1 || arg > REGS)
            return VM_WRONG_REGISTER_CODE;
        res += proc->regs->regs_arr[arg - 1];
        arg_p++;
        proc->PC += (int) sizeof(stack_elem_t);
    }
    if (cmd & 0x80) {
        if (res < 0 || res > proc->RAM_len)
            return VM_RAM_OUT_OF_BOUNDS;

        stack_push(proc->stack, proc->RAM[res], &err);
        if (err != STACK_OK)
            return VM_STACK_ERROR;

        return VM_OK;
    }

    stack_push(proc->stack, res, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

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

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    fprintf(stdout, "%" ELEM_SPEC "\n", value);

    return VM_OK;
}


int vm_do_POP(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->regs != NULL);

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    char cmd = proc->code[proc->PC];
    stack_elem_t res = 0;
    ssize_t err = STACK_OK;

    stack_elem_t *arg_p = (stack_elem_t *) (proc->code + proc->PC + 1);

    if ((cmd & 0x20) && !(cmd & 0x80)) {
        return VM_POP_CONST;
    }
    if ((cmd & 0x20) && (cmd & 0x80)) {
        GET_ARG(arg_p)
        res += arg;
        arg_p++;
        proc->PC += (int) sizeof(stack_elem_t);
    }
    if ((cmd & 0x40) && !(cmd & 0x80)) {
        GET_ARG(arg_p)
        if (REGS < arg - 1 || arg <= 0) {
            return VM_WRONG_REGISTER_CODE;
        }
        res += arg;
        arg_p++;
        proc->PC += (int) sizeof(stack_elem_t);
        stack_pop(proc->stack, &proc->regs->regs_arr[res - 1], &err);
    }
    if ((cmd & 0x40) && (cmd & 0x80)) {
        GET_ARG(arg_p)
        if (REGS < arg - 1 || arg <= 0) {
            return VM_WRONG_REGISTER_CODE;
        }
        res += proc->regs->regs_arr[arg - 1];
        arg_p++;
        proc->PC += (int) sizeof(stack_elem_t);
    }
    if (cmd & 0x80) {
        if (res > proc->RAM_len || res <= 0) {
            return VM_RAM_OUT_OF_BOUNDS;
        }
        stack_pop(proc->stack, &proc->RAM[res], &err);
    }

    if (err != STACK_OK)
        return VM_STACK_ERROR;

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

    if (err != STACK_OK)
        return VM_STACK_ERROR;

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

    if (err != STACK_OK)
        return VM_STACK_ERROR;

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

    if (err != STACK_OK)
        return VM_STACK_ERROR;

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

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    return VM_OK;
}


int vm_do_JMP(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    proc->PC = arg - 1;

    return VM_OK;
}


int vm_do_JA(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    if (val1 > val2) proc->PC = arg - 1;

    else proc->PC += (int) sizeof(stack_elem_t);

    return VM_OK;
}


int vm_do_JAE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    if (val1 >= val2) proc->PC = arg - 1;

    else proc->PC += (int) sizeof(stack_elem_t);

    return VM_OK;
}


int vm_do_JB(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    if (val1 < val2) proc->PC = arg - 1;

    else proc->PC += (int) sizeof(stack_elem_t);

    return VM_OK;
}


int vm_do_JBE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    if (val1 <= val2) proc->PC = arg - 1;

    else proc->PC += (int) sizeof(stack_elem_t);

    return VM_OK;
}


int vm_do_JE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    if (val1 == val2) proc->PC = arg - 1;

    else proc->PC += (int) sizeof(stack_elem_t);

    return VM_OK;
}


int vm_do_JNE(struct VM_processor *proc) {
    assert(proc != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))

    ssize_t err = STACK_OK;
    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(proc->stack, &val1, &err);
    stack_pop(proc->stack, &val2, &err);

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    if (val1 != val2) proc->PC = arg - 1;

    else proc->PC += (int) sizeof(stack_elem_t);

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

    if (err != STACK_OK)
        return VM_STACK_ERROR;

    return VM_OK;
}


int vm_do_CALL(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->call_stack != NULL);
    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)
    UPDATE_STACK_DEBUG(proc->call_stack)

    GET_ARG((stack_elem_t *) (proc->code + proc->PC + 1))
    ssize_t call_err = STACK_OK;

    stack_push(proc->call_stack, proc->PC + (int) sizeof(stack_elem_t), &call_err);

    if (call_err != STACK_OK)
        return VM_CALL_STACK_ERROR;

    proc->PC = arg - 1; // to next cmd at arg position

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

    if (call_err != STACK_OK)
        return VM_CALL_STACK_ERROR;

    return VM_OK;
}


int vm_do_DRAW(struct VM_processor *proc) {
    assert(proc != NULL);

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    vm_mem_dump(proc);

    return VM_OK;
}


int vm_do_HLT(struct VM_processor *proc) {
    assert(proc != NULL);

    UPDATE_VM_DEBUG(proc)

    return VM_HALT;
}
