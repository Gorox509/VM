#include "compiler.hpp"
#include "compiler_make.hpp"

#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#define GET_CMD(...) #__VA_ARGS__


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error: wrong arguments");
        return 1;
    }
    char *file_in  = argv[1];
    char *file_out = argv[2];

    struct stat file_stat = {};
    stat(file_in, &file_stat);

    struct compiler *comp = make_compiler_struct((size_t) file_stat.st_size + 2);

    FILE *fp_in = fopen(file_in, "rb");
    compiler_read_file_to_buffer(fp_in, comp, (size_t) file_stat.st_blksize);
    fclose(fp_in);

    compiler_error(compiler_do_compiling(comp));
    compiler_error(compiler_do_compiling(comp));

    FILE *fp_out = fopen(file_out, "wb");
    compiler_write_buffer_to_file(fp_out, comp);
    fclose(fp_out);

    compiler_destruct(comp);
    free(comp);

    return 0;
}


int compiler_read_file_to_buffer(FILE *fp, struct compiler *comp, size_t block_size) {

    assert(fp != NULL);
    assert(comp != NULL);
    assert(comp->input_buf != NULL);
    assert(block_size != 0);

    int n_read = 0;
    int n_lines = 0;

    while (!feof(fp)) {
        int n_fread = (int) fread(comp->input_buf + n_read, sizeof(char), (size_t) block_size, fp);

        for (int i = 0; i < n_fread; ++i) {
            if (comp->input_buf[n_read + i] == '\n' || comp->input_buf[n_read + i] == '\0') {
                ++n_lines;
            }
        }

        n_read += n_fread;
    }
    comp->input_buf[++n_read] = '\0';

    return n_lines;
}


int compiler_do_compiling(struct compiler *comp) {
    assert(comp != NULL);
    assert(comp->code_buf != NULL);
    assert(comp->input_buf != NULL);
    assert(comp->labels != NULL);

    comp->code_len = 0;

    int n_read = 0;
    int n_written = 0;

    char cmd[256] = "";
    char arg[256] = "";

    while (comp->input_buf[n_read] != '\0') {
        int n_scanned = 0;
        int n_scanned_arg = 0;
        if (sscanf(comp->input_buf + n_read, "%s %n", cmd, &n_scanned) != 1) {
            return COMPILER_CANT_SCAN_CMD;
        }

        n_read += n_scanned;

        if (!is_known_cmd(cmd)) {
            return COMPILER_UNKNOWN_CMD;
        }

        if (is_label(cmd)) {
            int n_label = 0;
            int err = get_label_num(cmd, &n_label);
            if (err != COMPILER_OK) return err;
            if ((err = compiler_add_label(comp, n_label, n_written - 1)) != COMPILER_OK) return err;
            continue;
        }

        comp->code_buf[n_written++] = command_code_from_str(cmd);

        if (has_argument(cmd)) {
            if (sscanf(comp->input_buf + n_read, "%s %n", arg, &n_scanned_arg) != 1) {
                return COMPILER_CANT_SCAN_ARG;
            }
            n_read += n_scanned_arg;

            if (is_label(arg)) {
                int n_label = 0;
                int err = COMPILER_OK;
                if ((err = get_label_num(arg, &n_label)) != COMPILER_OK)
                    return err;

                if ((err = compiler_put_label(comp, n_label, n_written++)) != COMPILER_OK)
                    return err;
                continue;
            }

            comp->code_buf[n_written++] = argument_from_str(arg);
        }
    }

    comp->code_len = n_written;

    return COMPILER_OK;
}


int compiler_write_buffer_to_file(FILE *fp_out, struct compiler *comp) {
    assert(fp_out != NULL);
    assert(comp != NULL);
    assert(comp->code_buf != NULL);

    fwrite(comp->code_buf, sizeof(int), (size_t) comp->code_len, fp_out);

    return COMPILER_OK;
}


int compiler_add_label(struct compiler *comp, int n_label, int pos) {
    assert(comp != NULL);
    assert(comp->labels != NULL);

    if (comp->labels_size < n_label)
        return COMPILER_LABEL_OUTSIDE_LABELS_ARRAY;

    if (comp->labels[n_label] != EMPTY_LABEL && comp->labels[n_label] != pos) {
        return COMPILER_LABEL_REDEFINITION;
    }

    comp->labels[n_label] = pos;

    return COMPILER_OK;
}


int compiler_put_label(struct compiler *comp, int n_label, int pos) {
    assert(comp != NULL);
    assert(comp->labels != NULL);
    assert(comp->code_buf != NULL);

    comp->code_buf[pos] = comp->labels[n_label];

    return COMPILER_OK;
}


int get_label_num(char *arg, int *n_label) {
    assert(arg != NULL);
    assert(n_label != NULL);

    if ((*n_label = (int) strtol(arg + 1, NULL, 10)) == 0 && arg[0] != '0') { // TODO: redo. cringe
        return COMPILER_WRONG_LABEL;
    }

    return COMPILER_OK;
}


int command_code_from_str(char *str) {
    assert(str != NULL);

    if (!strcmp(str, GET_CMD(PUSH))) return PUSH;
    if (!strcmp(str, GET_CMD(ADD))) return ADD;
    if (!strcmp(str, GET_CMD(SUB))) return SUB;
    if (!strcmp(str, GET_CMD(MUL))) return MUL;
    if (!strcmp(str, GET_CMD(DIV))) return DIV;
    if (!strcmp(str, GET_CMD(OUT))) return OUT;
    if (!strcmp(str, GET_CMD(HLT))) return HLT;
    if (!strcmp(str, GET_CMD(PUSHR))) return PUSHR;
    if (!strcmp(str, GET_CMD(POPR))) return POPR;
    if (!strcmp(str, GET_CMD(JMP))) return JMP;
    if (!strcmp(str, GET_CMD(JA))) return JA;
    if (!strcmp(str, GET_CMD(JAE))) return JAE;
    if (!strcmp(str, GET_CMD(JB))) return JB;
    if (!strcmp(str, GET_CMD(JBE))) return JBE;
    if (!strcmp(str, GET_CMD(JE))) return JE;
    if (!strcmp(str, GET_CMD(JNE))) return JNE;
    if (!strcmp(str, GET_CMD(IN))) return IN;
    if (!strcmp(str, GET_CMD(CALL))) return CALL;
    if (!strcmp(str, GET_CMD(RET))) return RET;

    return UNKNOWN;
}


int argument_from_str(char *str) {
    assert(str != NULL);

    return atoi(str);
}


int is_label(char *str) {
    assert(str != NULL);

    if (str[0] == ':')
        return 1;
    return 0;
}


int is_known_cmd(char *cmd) { // TODO: to ?array of names or something idk?
    assert(cmd != NULL);

    return (
        cmd[0] == ';' ||
        cmd[0] == ':' ||
        !strcmp(cmd, "PUSH") ||
        !strcmp(cmd, "ADD") ||
        !strcmp(cmd, "SUB") ||
        !strcmp(cmd, "MUL") ||
        !strcmp(cmd, "DIV") ||
        !strcmp(cmd, "OUT") ||
        !strcmp(cmd, "HLT") ||
        !strcmp(cmd, "PUSHR") ||
        !strcmp(cmd, "POPR") ||
        !strcmp(cmd, "JMP") ||
        !strcmp(cmd, "JA") ||
        !strcmp(cmd, "JAE") ||
        !strcmp(cmd, "JB") ||
        !strcmp(cmd, "JBE") ||
        !strcmp(cmd, "JE") ||
        !strcmp(cmd, "JNE") ||
        !strcmp(cmd, "IN") ||
        !strcmp(cmd, "CALL") ||
        !strcmp(cmd, "RET")
    );
}


int has_argument(char *cmd) {
    assert(cmd != NULL);

    return (
        !strcmp(cmd, "PUSH") ||
        !strcmp(cmd, "PUSHR") ||
        !strcmp(cmd, "POPR") ||
        !strcmp(cmd, "CALL") ||
        !strcmp(cmd, "JMP") ||
        !strcmp(cmd, "JA") ||
        !strcmp(cmd, "JAE") ||
        !strcmp(cmd, "JB") ||
        !strcmp(cmd, "JBE") ||
        !strcmp(cmd, "JE") ||
        !strcmp(cmd, "JNE")
    );
}


int compiler_error(int error) {
    switch (error) {
        default:
        case COMPILER_OK:
            break;

        case COMPILER_CANT_SCAN_CMD:
            fprintf(stderr, "Error: Can not scan command\n");
            break;

        case COMPILER_CANT_SCAN_ARG:
            fprintf(stderr, "Error: Can not scan argument\n");
            break;

        case COMPILER_UNKNOWN_CMD:
            fprintf(stderr, "Error: Unknown command\n");
            break;

        case COMPILER_LABEL_REDEFINITION:
            fprintf(stderr, "Error: Label redefiniton\n");
            break;

        case COMPILER_LABEL_OUTSIDE_LABELS_ARRAY:
            fprintf(stderr, "Error: Label value is unsupported\n");
            break;

        case COMPILER_WRONG_LABEL:
            fprintf(stderr, "Error: wrong label format\n");
            break;
    }

    return COMPILER_OK;
}
