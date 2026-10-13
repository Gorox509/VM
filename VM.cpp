#include <cstdio>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#include "VM.hpp"
#include "VM_commands.hpp"
#include "stack/stack.hpp"

#define RED "\033[31m"
#define BASE_CLR "\033[0m"


int main(int argc, char *argv[]) {
    assert(argv != NULL);

    if (argc != 2) {
        fprintf(stderr, "Error: Wrong arguments\n");
        exit(1);
    }
    char *file_in = argv[1];

    struct stat file_stat = {};
    stat(file_in, &file_stat);

    FILE *fp = fopen(file_in, "rb");

    if (fp == NULL) {
        fprintf(stderr, "Error: Can not open the file\n");
        return 1;
    }

    struct VM_processor *proc = vm_make_processor((size_t) file_stat.st_size + 2);

    char *buf = (char *) malloc((size_t) file_stat.st_size + 2);
    if (buf == NULL) {
        fprintf(stderr, "Error: failed to allocate buffer\n");
        return 1;
    }

    vm_read_file_to_buffer(fp, buf, (size_t) file_stat.st_blksize);
    fclose(fp);

    vm_translate_buffer_to_code(proc, buf, (size_t) file_stat.st_size + 2);
    free(buf);

    vm_do_program(proc);

    vm_processor_destructor(proc);
    free(proc);

    return 0;
}


struct VM_processor *vm_make_processor(size_t len) {
    struct VM_processor *proc = vm_make_proc_core();
    assert(proc != NULL);

    vm_make_proc_stack(proc);

    vm_make_proc_code(proc, len);

    vm_make_proc_regs(proc);

    ON_VM_DEBUG(
        vm_make_proc_error_struct(proc);
    )
    UPDATE_VM_DEBUG(proc)
    return proc;
}


struct VM_processor *vm_make_proc_core() {
    struct VM_processor *proc = (struct VM_processor *) malloc(sizeof(VM_processor));
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

    proc->regs = (struct VM_registers *) malloc(sizeof(VM_registers));
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
    free(proc->code);

    free(proc->regs);

    ON_VM_DEBUG(
        free(proc->error);
    )

    return VM_OK;
}


int vm_processor_dump(struct VM_processor *proc) {
    if (proc == NULL) {
        fprintf(stderr, "Dumping program is unsafe\n");
        return VM_ZERO_PTR;
    }

    fprintf(stderr, "Dumping processor structure:\n");
    ON_VM_DEBUG(
        fprintf(stderr, "Virtual processor structure, last called from function \"%s\" in %s:%d\n", proc->error->last_called, proc->error->filename, proc->error->line);
        fprintf(stderr, "Code lines read: %d\n", proc->error->code_line);
    )
    fprintf(stderr, "Current program counter: % d\n", proc->PC);
    if (proc->code == NULL) {
        fprintf(stderr, "Code array can not be printed");
    }
    else {
        fprintf(stderr, "[ ");
        for (int i = 0; i < proc->code_len; ++i) {
            fprintf(stderr, "%3d,", proc->code[i]);
        }
        fprintf(stderr, "]\n");
        for (int i = -1; i < proc->PC && i < proc->code_len; ++i) fprintf(stderr, "    ");
        fprintf(stderr, "^\n");
    }

    fprintf(stderr, "Register values:\n");

    for (int i = 0; i < proc->regs->len; ++i) {
        fprintf(stderr, "register r%dx: 0x%08x\n", i + 1, *((unsigned int *) (proc->regs->regs_arr + i)));
    }

    stack_dump(proc->stack);

    return VM_OK;
}


void put_divisor(FILE *fp) {
    assert(fp != NULL);

    fprintf(fp, "======================================================================"
                "==============================\n");
}


int vm_do_program(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->code != NULL);

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    int err = VM_OK;
    while (proc->PC < proc->code_len) {
        err = vm_do_instruction(proc);

        if (err == VM_HALT)
            return VM_OK;

        if (err != VM_OK)
            vm_handle_error(proc, err);

        proc->PC++;
    }
    return err;
}


int vm_do_instruction(struct VM_processor *proc) {
    assert(proc != NULL);
    assert(proc->stack != NULL);
    assert(proc->code != NULL);

    UPDATE_VM_DEBUG(proc)
    UPDATE_STACK_DEBUG(proc->stack)

    stack_elem_t arg = 0;
    int cmd = proc->code[proc->PC];

    if (cmd == PUSH || cmd == POPR || cmd == PUSHR)
        arg = proc->code[++proc->PC];

    ssize_t stack_err = STACK_OK;

    switch (cmd) {
        case PUSH:
            return vm_do_push(proc, arg, &stack_err);

        case PUSHR:
            return vm_do_push_r(proc, arg, &stack_err);

        case POPR:
            return vm_do_pop_r(proc, arg, &stack_err);

        case OUT:
            return vm_do_out(proc, &stack_err);

        case ADD:
            return vm_do_add(proc);

        case SUB:
            return vm_do_sub(proc);

        case MUL:
            return vm_do_mult(proc);

        case DIV:
            return vm_do_div(proc);

        case HLT:
            return VM_HALT;

        default:
            return VM_NO_COMMAND_FOUND;
    }

    return VM_NO_COMMAND_FOUND;
}


int vm_handle_error(struct VM_processor *proc, int error) {
    if (error != VM_OK) {
        put_divisor(stderr);
        fprintf(stderr, RED "Runtime error: ");
    }

    switch (error) {
        case VM_NO_COMMAND_FOUND:
            fprintf(stderr, "No command found\n");
            break;

        case VM_WRONG_COMMAND:
            fprintf(stderr, "Wrong command, can not start running\n");
            break;

        case VM_ZERO_PTR:
            fprintf(stderr, "Virtual processor structure pointer is zero\n");
            break;

        case VM_WRONG_REGISTER_CODE:
            fprintf(stderr, "Wrong register code\n");
            break;

        case VM_DIV_BY_ZERO:
            fprintf(stderr, "Error: dividing by zero\n");
            break;

        default:
            break;
    }
    fprintf(stderr, BASE_CLR);
    if (error != VM_OK) {
        vm_processor_dump(proc);
        put_divisor(stderr);
        exit(1);
    }

    return error;
}


int vm_read_file_to_buffer(FILE *fp, char *buf, size_t block_size) {
    assert(fp != NULL);
    assert(buf != NULL);

    int n_read = 0;

    while (!feof(fp)) {
        n_read += (int) fread(buf + n_read, sizeof(char), block_size, fp);
    }
    buf[++n_read] = '\0';


    return VM_OK;
}


int vm_translate_buffer_to_code(struct VM_processor *proc, char *buf, size_t size) {
    assert(buf != NULL);
    assert(proc != NULL);
    assert(proc->code != NULL);

    UPDATE_VM_DEBUG(proc)

    int size_read = 0;

    while (size_read < (ssize_t) size && buf[size_read] > '\0') {
        int read = 0;
        int n_read = 0;

        proc->code_len += n_read = sscanf(buf + size_read, "%d %n", &proc->code[proc->code_len], &read); // TODO: custom function that counts read symbols and newlines
        size_read += read;

        if (n_read == 0 && buf[size_read] > '\0')
            return vm_handle_error(proc, VM_WRONG_COMMAND);

        ON_VM_DEBUG(
            if (proc->code[proc->code_len - n_read] != PUSH)
                proc->error->code_line += 1;
        )
    }

    return VM_OK;
}


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
