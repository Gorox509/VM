#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#include "VM.hpp"
#include "../VM_commands.hpp"
#include "VM_command_funcs.hpp"
#include "VM_reading.hpp"
#include "VM_proc_make_funcs.hpp"

#define RED "\033[31m"
#define BASE_CLR "\033[0m"


int main(int argc, char *argv[]) {
    assert(argv != NULL);

    if (argc != 2 && argc != 3) {
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

    if (argc == 3 && !strcmp(argv[2], "text"))
        proc->text_mode = 1;

    if (proc->text_mode) {
        char *buf = (char *) malloc((size_t) file_stat.st_size + 2);
        if (buf == NULL) {
            fprintf(stderr, "Error: failed to allocate buffer\n");
            return 1;
        }

        vm_read_file_to_buffer(fp, buf, (size_t) file_stat.st_blksize);
        fclose(fp);

        vm_translate_buffer_to_code(proc, buf, (size_t) file_stat.st_size + 2);

        free(buf);
    }

    else {
        vm_read_binary_code(fp, proc, (size_t) file_stat.st_size);
    }

    vm_do_program(proc);

    vm_processor_destructor(proc);
    free(proc);

    return 0;
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
        fprintf(stderr, "[  ");
        for (int i = 0; i < proc->code_len; ++i) {
            if (i == proc->PC) fprintf(stderr, RED);
            fprintf(stderr, "%hhx%hhx, ", proc->code[i] >> 4, proc->code[i] & 0x0f);
            if (i == proc->PC) fprintf(stderr, BASE_CLR);
        }
        fprintf(stderr, "]\n");
        for (int i = -1; i < proc->PC && i < proc->code_len; ++i) fprintf(stderr, "    ");
        fprintf(stderr, "^\n");
    }

    fprintf(stderr, "Register values:\n");

    for (int i = 0; i < REGS; ++i) {
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

    if (proc->PC < 0 || proc->PC > proc->code_len) {
        return VM_WRONG_PC;
    }

    switch (proc->code[proc->PC] & 0x1f) {

        #define GENERATE(cmd)   case cmd:                               \
                                    return vm_do_ ## cmd(proc);

        #include "../generate.txt"

        #undef GENERATE

        case UNKNOWN:
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

        case VM_WRONG_PC:
            fprintf(stderr, "Error: Program Counter value is invalid\n");
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
