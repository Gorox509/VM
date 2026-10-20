#include "compiler_make.hpp"
#include "compiler.hpp"
#include <stdio.h>
#include <assert.h>


struct compiler *make_compiler_struct(size_t len) {
    struct compiler *comp = make_compiler_core();

    make_compiler_buffer(comp, len);

    make_compiler_code(comp);

    make_compiler_labels(comp);

    return comp;
}


struct compiler *make_compiler_core() {
    struct compiler *comp = (struct compiler *) malloc(sizeof(struct compiler));
    if (comp == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1); // TODO: to return err;
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

    comp->code_size = 1024;
    comp->code_buf = (char *) calloc((size_t) comp->code_size, sizeof(char)); // TODO: extend when needed
    if (comp->code_buf == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }

    comp->code_len = 0;

    return COMPILER_OK;
}


int make_compiler_labels(struct compiler *comp) {
    assert(comp != NULL);

    comp->labels_size = 1024;
    comp->labels = (int *) calloc((size_t) comp->labels_size, sizeof(int));
    if (comp->labels == NULL) {
        fprintf(stderr, "Error: failed to allocate memory\n");
        exit(1);
    }
    for (int i = 0; i < comp->labels_size; ++i) comp->labels[i] = EMPTY_LABEL;



    return COMPILER_OK;
}


int compiler_destruct(struct compiler *comp) {
    assert(comp != NULL);

    free(comp->code_buf);
    free(comp->input_buf);
    free(comp->labels);


    return COMPILER_OK;
}
