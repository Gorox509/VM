#include "VM_reading.hpp"
#include <assert.h>

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
