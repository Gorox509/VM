#include "compiler.hpp"
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error: wrong arguments");
    }
    char *file_in  = argv[1];
    char *file_out = argv[2];

    int err = COMPILER_OK;

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

    err = compiler_read_file_to_buffer(fp_in, buf, (size_t) file_stat.st_blksize);
    compiler_error(err);

    fclose(fp_in);

    char *buf_compiled = (char *) malloc((size_t) file_stat.st_size + 2);
    if (buf_compiled == NULL) {
        fprintf(stderr, "Error: fail to allocate memory");
        return 1;
    }
    err = compiler_do_compiling_from_buffer_to_new_buffer(buf, buf_compiled);
    compiler_error(err);

    FILE *fp_out = fopen(file_out, "wb");

    err = compiler_write_buffer_to_file(fp_out, buf_compiled);
    compiler_error(err);


    fclose(fp_out);

    free(buf_compiled);
    free(buf);

    return 0;
}


int compiler_read_file_to_buffer(FILE *fp, char *buffer, size_t block_size) {
    assert(fp != NULL);
    assert(buffer != NULL);
    assert(block_size != 0);

    int n_read = 0;

    while (!feof(fp)) {
        n_read += (int) fread(buffer + n_read, sizeof(char), block_size, fp);
    }
    buffer[++n_read] = '\0';

    return COMPILER_OK;
}
