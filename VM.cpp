#include <cctype>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>

#include "stack/stack.hpp"

enum VM_errors {
    VM_OK = 0,
    VM_NO_COMMAND_FOUND,

};

enum VM_commands {
    PUSH = 1,
    ADD,
    SUB,
    MULT,
    DIV,
    OUT,
    HLT,
};

int vm_do_program(struct stack *stk, char *buf);
int vm_do_instruction(struct stack *stk, char *buf, size_t *PC);
int vm_handle_error(int error);
int vm_read_file_to_buffer(FILE *fp, char *buffer, size_t block_size);

void vm_do_push(struct stack *stk, stack_elem_t value, ssize_t *err);
void vm_do_out(struct stack *stk);
void vm_do_add(struct stack *stk);
void vm_do_sub(struct stack *stk);
void vm_do_mult(struct stack *stk);
void vm_do_div(struct stack *stk);


int main(int argc, char *argv[]) {
    assert(argv != NULL);

    if (argc != 2) {
        fprintf(stderr, "Error: Wrong arguments\n");
        return 1;
    }
    char *file_in = argv[1];

    struct stat file_stat = {};
    stat(file_in, &file_stat);

    FILE *fp = fopen(file_in, "rb");

    if (fp == NULL) {
        fprintf(stderr, "Error: Can not open the file\n");
    }
    char *buf = (char *) malloc((size_t) file_stat.st_size + 2);
    if (buf == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        return 1;
    }

    vm_read_file_to_buffer(fp, buf, (size_t) file_stat.st_blksize);
    fclose(fp);

    struct stack *stk = (struct stack *) malloc(sizeof(struct stack));
    if (stk == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        return 1;
    }
    *stk = {};
    stack_constructor(stk, 10 ON_DEBUG(, __FILE_NAME__, __LINE__, "stk"));

    int err = vm_do_program(stk, buf);

    vm_handle_error(err);

    stack_destructor(stk);
    free(stk);
    free(buf);

    return 0;
}


int vm_do_program(struct stack *stk, char *buf) {
    assert(stk != NULL);
    assert(buf != NULL);

    int err = VM_OK;
    size_t PC = 0;

    while (buf[PC] != '\0') {
        err = vm_do_instruction(stk, buf, &PC);

        if (err != VM_OK)
            return err;

    }
    return err;
}


int vm_do_instruction(struct stack *stk, char *buf, size_t *PC) {
    assert(stk != NULL);
    assert(buf != NULL);
    assert(PC != NULL);

    stack_elem_t arg = 0.;
    int cmd = 0;
    char ch = 0;

    sscanf(buf + *PC, "%d", &cmd);
    while (!isspace(ch = buf[(*PC)++]) && ch != '\0');


    if (cmd == PUSH) {
        sscanf(buf + *PC, "%lg", &arg);
        while (!isspace(ch = buf[(*PC)++]) && ch != '\0');
    }
    if (ch == '\0') {
        (*PC)--;
        return VM_OK;
    }

    ssize_t stack_err = STACK_OK;

    switch (cmd) {
        case PUSH:
            vm_do_push(stk, arg, &stack_err);
            return VM_OK;

        case OUT:
            vm_do_out(stk);
            return VM_OK;

        case ADD:
            vm_do_add(stk);
            return VM_OK;

        case SUB:
            vm_do_sub(stk);
            return VM_OK;

        case MULT:
            vm_do_mult(stk);
            return VM_OK;

        case DIV:
            vm_do_div(stk);
            return VM_OK;

        case HLT:
            return VM_OK;

        default:
            return VM_NO_COMMAND_FOUND;
    }

    return VM_NO_COMMAND_FOUND;
}


int vm_handle_error(int error) {
    if (error != VM_OK)
        fprintf(stderr, "Runtime error: ");

    switch (error) {
        case VM_NO_COMMAND_FOUND:
            fprintf(stderr, "No command found\n");
            break;

        default:
            break;
    }
    return error;
}


int vm_read_file_to_buffer(FILE *fp, char *buffer, size_t block_size) {
    assert(fp != NULL);
    assert(buffer != NULL);
    assert(block_size != 0);

    int n_read = 0;

    while (!feof(fp)) {
        n_read += (int) fread(buffer + n_read, sizeof(char), block_size, fp);
    }
    buffer[++n_read] = '\0';

    return n_read;
}


void vm_do_push(struct stack *stk, stack_elem_t value, ssize_t *err) {
    assert(stk != NULL);
    assert(err != NULL);

    stack_push(stk, value, err);
}


void vm_do_out(struct stack *stk) {
    assert(stk != NULL);

    stack_elem_t value = 0;
    ssize_t err = STACK_OK;

    stack_pop(stk, &value, &err);
    stack_push(stk, value, &err);
    fprintf(stdout, "%lg\n", value);
}


void vm_do_add(struct stack *stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(stk, &val1, &err);
    stack_pop(stk, &val2, &err);

    stack_push(stk, val1 + val2, &err);
}


void vm_do_sub(struct stack *stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(stk, &val1, &err);
    stack_pop(stk, &val2, &err);

    stack_push(stk, val1 - val2, &err);
}


void vm_do_mult(struct stack *stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(stk, &val1, &err);
    stack_pop(stk, &val2, &err);

    stack_push(stk, val1 * val2, &err);
}


void vm_do_div(struct stack *stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    stack_elem_t val1 = 0, val2 = 0;

    stack_pop(stk, &val1, &err);
    stack_pop(stk, &val2, &err);

    if (val2 == 0) {
        fprintf(stderr, "Error: dividing by zero\n");
        return;
    }

    stack_push(stk, val1 / val2, &err);
}
