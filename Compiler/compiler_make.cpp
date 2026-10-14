#include "compiler_make.hpp"
#include "compiler.hpp"
#include <stdio.h>
#include <assert.h>


struct compiler *make_compiler_struct(size_t len) {
    struct compiler *comp = make_compiler_core();

    make_compiler_buffer(comp, len);

    make_compiler_code(comp);

    make_compiler_marks(comp);

    return comp;
}


struct compiler *make_compiler_core() {
    struct compiler *comp = (struct compiler *) malloc(sizeof(struct compiler));
    if (comp == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }
    *comp = {};

    return comp;
}


int make_compiler_buffer(struct compiler *comp, size_t len) {
    assert(comp != NULL);
    assert(len != 0);

    comp->input_buf = (char *) calloc(len, sizeof(char));
    if (comp->input_buf == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }

    return COMPILER_OK;
}


int make_compiler_code(struct compiler *comp) {
    assert(comp != NULL);

    comp->code_buf = (int *) calloc(32, sizeof(int)); // TODO: extend when needed
    if (comp->code_buf == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }
    comp->code_size = 32;
    comp->code_len = 0;

    return COMPILER_OK;
}


int make_compiler_marks(struct compiler *comp) {
    assert(comp != NULL);

    comp->labels = (int *) calloc(32, sizeof(int));
    if (comp->labels == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }
    for (int i = 0; i < 32; ++i) comp->labels[i] = EMPTY_LABEL;

    comp->labels_size = 32;

    return COMPILER_OK;
}


int compiler_destruct(struct compiler *comp) {
    assert(comp != NULL);

    free(comp->code_buf);
    free(comp->input_buf);
    free(comp->labels);

    return COMPILER_OK;
}
