#include "compiler.hpp"
#include "VM_commands.hpp""
#include <cstdio>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error: wrong arguments");
    }
    char *file_in  = argv[1];
    char *file_out = argv[2];


    struct stat file_stat = {};
    stat(file_in, &file_stat);

    FILE *fp_in = fopen(file_in, "rb");
    if (fp_in == NULL) {
        fprintf(stderr, "Error: cant open the file for reading");
        return 1;
    }
    char *buf = (char *) malloc((size_t) file_stat.st_size + 1);
    if (buf == NULL) {
        fprintf(stderr, "Error: fail to allocate memory");
        return 1;
    }

    int n_lines = compiler_read_file_to_buffer(fp_in, buf, (size_t) file_stat.st_blksize);

    fclose(fp_in);

    char *buf_compiled = (char *) malloc((size_t) file_stat.st_size + 2);
    if (buf_compiled == NULL) {
        fprintf(stderr, "Error: fail to allocate memory");
        return 1;
    }
    size_t size_compiled = compiler_do_compiling_from_buffer_to_new_buffer(buf, buf_compiled);

    FILE *fp_out = fopen(file_out, "wb");

    compiler_write_buffer_to_file(fp_out, buf_compiled);

    fclose(fp_out);

    free(buf_compiled);
    free(buf);

    return 0;
}


int read_lines_from_file_to_buffer(FILE *fp, char *buffer, const __blksize_t block_size) {

    assert(fp != NULL);
    assert(buffer != NULL);
    assert(block_size != 0);

    int n_read = 0;
    int n_lines = 0;

    while (!feof(fp)) {
        int n_fread = fread(buffer + n_read, sizeof(char), (size_t) block_size, fp);

        for (int i = 0; i < n_fread; ++i) {
            if (buffer[n_read + i] == '\n' || buffer[n_read + i] == '\0') {
                ++n_lines;
            }
        }

        n_read += n_fread;
    }
    buffer[++n_read] = '\0';

    return n_lines;
}


int compiler_do_compiling_from_buffer_to_new_buffer(char *buf, char *buf_new) {
    return 0;
}


int compiler_error(int error) {
    switch (error) {
        default:
        case COMPILER_OK:
            break;

    }

    return COMPILER_OK;
}


int compiler_write_buffer_to_file(FILE *fp_out, char *buf_compiled) {
    assert(fp_out != NULL);
    assert(buf_compiled != NULL);

}
