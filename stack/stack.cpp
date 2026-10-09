#include "stack.hpp"

#define GET_CANARY(ptr) (0xDEFEC8ED^((size_t) ptr))
#define GET_DATA_CANARY(type, ptr) ((type) (0xDEFEC8ED^((size_t) ptr)))
#define ERROR_CODE_TO_CODE_AND_TEXT(code) code, #code

#define RED "\033[31m"
#define BASE_CLR "\033[0m"

#include <assert.h>
#include <stdio.h>


static ssize_t stack_apply_error_and_dump
                            (struct         stack *stk,       ssize_t      err_code,    const char *err_msg);


ssize_t stack_error(struct stack *const stk) {
    assert(stk != NULL);


    if (stk->data_allocated && stk->data == NULL && (stk->size != 0 || stk->capacity != 0)) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_DATA_PTR));
    }

    if (stk->size == (size_t) -1) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_UNDERFLOW));
    }

    if (stk->size > stk->capacity) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_OVERFLOW));
    }

    if (stk->capacity == 0 && stk->data != NULL) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_CAPACITY));
    }


    ON_HASH_PROT(
    if (stk->data_allocated && stk->hash_struct->hash != stack_calculate_hash(stk)) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_HASH));
    }

    if (stk->data_allocated && stk->hash_struct->data_hash != stack_calculate_data_hash(stk)) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_DATA_HASH));
    }
    )

    ON_CANARY_PROT(
    if (stk->left_canary != GET_CANARY(&stk->left_canary)) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_LEFT_CANARY));
    }

    if (stk->right_canary != GET_CANARY(&stk->right_canary)) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_RIGHT_CANARY));
    }

    if (stk->data_allocated && !doubles_equal(stk->data[0], GET_DATA_CANARY(stack_elem_t, &stk->data[0]))) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_DATA_LEFT_CANARY));
    }

    if (stk->data_allocated && !doubles_equal(stk->data[stk->capacity + 1], GET_DATA_CANARY(stack_elem_t, &stk->data[stk->capacity + 1]))) {
        return stack_apply_error_and_dump(stk, ERROR_CODE_TO_CODE_AND_TEXT(STACK_WRONG_DATA_RIGHT_CANARY));
    }
    )


    ON_DEBUG(
    if (stk->data_allocated && stk->err_struct != NULL) {
        stk->err_struct->error = STACK_OK;
        stk->err_struct->err_name = "STACK_OK";
    }
    )

    ON_HASH_PROT(if (stk->data_allocated && stk->hash_struct != NULL) stack_update_hash(stk);)

    return STACK_OK;
}



ssize_t stack_apply_error_and_dump(struct stack *const stk, ssize_t err_code, const char *err_msg) {
    assert(stk != NULL);
    assert(err_msg != NULL);

    ON_DEBUG(
    if (stk->err_struct != NULL && stk->data_allocated) {
        stk->err_struct->error = err_code;
        stk->err_struct->err_name = err_msg;
    }
    )

    ON_HASH_PROT(if (stk->hash_struct != NULL && stk->data_allocated) stk->hash_struct->hash = stack_calculate_hash(stk);)

    stack_dump(stk);

    return err_code;
}


ON_HASH_PROT(
size_t stack_calculate_hash(const struct stack *const stk) {
    assert(stk != NULL);

    return stack_djb2_hash(stk);
}
)


ON_HASH_PROT(
size_t stack_calculate_data_hash(const struct stack *const stk) {
    assert(stk != NULL);

    if (stk->data == NULL)
        return 0;

    return stack_djb2_data_hash(stk);
}
)


ON_HASH_PROT(
void stack_update_hash(struct stack *const stk) {
    assert(stk != NULL);

    stk->hash_struct->data_hash = stack_calculate_data_hash(stk);
    stk->hash_struct->hash = stack_calculate_hash(stk);
}
)


ON_CANARY_PROT(
ssize_t stack_assign_data_canaries(struct stack *const stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    assert(stack_error(stk) == STACK_OK);
    if ((err = stack_error(stk)) != STACK_OK)
        return err;

    ON_DEBUG(stk->err_struct->last_called = __func__;)
    ON_HASH_PROT(stack_update_hash(stk);)

    assert(stack_error(stk) == STACK_OK);
    if ((err = stack_error(stk)) != STACK_OK)
        return err;

    stk->data[0] = GET_DATA_CANARY(stack_elem_t, &stk->data[0]);
    stk->data[stk->capacity + 1] = GET_DATA_CANARY(stack_elem_t, &stk->data[stk->capacity + 1]);

    ON_HASH_PROT(stack_update_hash(stk);)

    assert(stack_error(stk) == STACK_OK);
    err = stack_error(stk);

    return err;
}
)


ssize_t stack_constructor(struct stack *const stk, size_t initial_size
                ON_DEBUG(,const char *filename, size_t line, const char *var_name))
{
    assert(stk != NULL);
    ON_DEBUG(stk->err_struct = (struct stack_err_struct *) malloc(sizeof(struct stack_err_struct));)
    ON_DEBUG(if (stk->err_struct == NULL) exit(1);)
    ON_DEBUG(stk->err_struct->last_called = __func__;)

    ON_HASH_PROT(stk->hash_struct = (struct stack_hash *) malloc(sizeof(struct stack_hash));)
    ON_HASH_PROT(if (stk->hash_struct == NULL) exit(1);)

    ON_CANARY_PROT(stk->left_canary = GET_CANARY(&stk->left_canary);)
    ON_CANARY_PROT(stk->right_canary = GET_CANARY(&stk->right_canary);)

    ssize_t err = STACK_OK;

    ON_HASH_PROT(stk->hash_struct->hash = stack_calculate_hash(stk);)

    assert(stack_error(stk) == STACK_OK);
    err = stack_error(stk);
    if (err != STACK_OK)
        return err;

    stk->data = (stack_elem_t *) calloc(initial_size ON_CANARY_PROT(+2), sizeof(stack_elem_t));

    stk->size = 0;
    stk->capacity = initial_size;

    ON_HASH_PROT(stack_update_hash(stk);)

    ON_CANARY_PROT(
    if ((err = stack_assign_data_canaries(stk)) != STACK_OK)
        return err;
    )
    stk->data_allocated = 1;

    ON_DEBUG(
    stk->err_struct->origin_filename = filename;
    stk->err_struct->origin_ptr = stk;
    stk->err_struct->var_name = var_name;
    stk->err_struct->line = line;
    )

    ON_HASH_PROT(stack_update_hash(stk);)

    assert(stack_error(stk) == STACK_OK);
    err = stack_error(stk);

    return err;
}


ssize_t stack_destructor(struct stack *const stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    assert(stack_error(stk) == STACK_OK);
    if ((err = stack_error(stk)) != STACK_OK)
        return err;


    ON_DEBUG(stk->err_struct->last_called = __func__;)
    ON_HASH_PROT(stack_update_hash(stk);)

    assert(stack_error(stk) == STACK_OK);
    err = stack_error(stk);

    if (err != STACK_OK)
        return err;

    free(stk->data);
    ON_DEBUG(free(stk->err_struct);)
    ON_HASH_PROT(free(stk->hash_struct);)
    stk->data = NULL;
    ON_DEBUG(stk->err_struct = NULL;)
    ON_HASH_PROT(stk->hash_struct = NULL;)
    stk->data_allocated = 0;

    assert(stack_error(stk) == STACK_OK);
    err = stack_error(stk);

    return err;
}


ssize_t stack_extend(struct stack *const stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    assert(stack_error(stk) == STACK_OK);
    if ((err = stack_error(stk)) != STACK_OK)
        return err;

    ON_DEBUG(stk->err_struct->last_called = __func__;)

    stk->data_allocated = 0;

    ON_HASH_PROT(stack_update_hash(stk);)
    if ((err = stack_error(stk)) != STACK_OK)
        return err;

    stk->capacity *= 2;
    stk->data = (stack_elem_t *) realloc((void *) stk->data, (stk->capacity ON_CANARY_PROT(+2)) * sizeof(stack_elem_t));

    ON_HASH_PROT(stack_update_hash(stk);)

    ON_CANARY_PROT(
    if ((err = stack_assign_data_canaries(stk)) != STACK_OK)
        return err;
    )

    stk->data_allocated = 1;

    ON_HASH_PROT(stack_update_hash(stk);)
    assert(stack_error(stk) == STACK_OK);
    if ((err = stack_error(stk)) != STACK_OK)
        return err;

    return err;
}


ssize_t stack_shrink(struct stack *const stk) {
    assert(stk != NULL);

    ssize_t err = STACK_OK;

    assert(stack_error(stk) == STACK_OK);
    if ((err = stack_error(stk)) != STACK_OK)
        return err;


    ON_DEBUG(stk->err_struct->last_called = __func__;)

    stk->data_allocated = 0;

    ON_HASH_PROT(stack_update_hash(stk);)
    if ((err = stack_error(stk)) != STACK_OK)
        return err;

    size_t new_capacity = stk->capacity / 2 + stk->capacity % 2;

    stk->data = (stack_elem_t *) realloc(stk->data, (new_capacity ON_CANARY_PROT(+2)) * sizeof(stack_elem_t));
    stk->capacity = new_capacity;

    ON_HASH_PROT(stack_update_hash(stk);)

    ON_CANARY_PROT(
    if ((err = stack_assign_data_canaries(stk)) != STACK_OK)
        return err;
    )

    stk->data_allocated = 1;

    ON_HASH_PROT(stack_update_hash(stk);)
    assert(stack_error(stk) == STACK_OK);
    if ((err = stack_error(stk)) != STACK_OK)
        return err;

    return err;
}


void stack_push(struct stack *const stk, stack_elem_t elem, ssize_t *err) {
    assert(stk != NULL);
    assert(err != NULL);

    assert(stack_error(stk) == STACK_OK);
    if ((*err = stack_error(stk)) != STACK_OK)
        return;

    ON_DEBUG(stk->err_struct->last_called = __func__;)

    ON_HASH_PROT(stack_update_hash(stk);)
    assert((*err = stack_error(stk)) == STACK_OK);
    if ((*err = stack_error(stk)) != STACK_OK)
        return;

    if (stk->size == stk->capacity) {
        if ((*err = stack_extend(stk)) != STACK_OK)
            return;
    }

    stk->data[stk->size++ ON_CANARY_PROT(+1)] = elem;

    ON_HASH_PROT(stack_update_hash(stk);)
    assert(stack_error(stk) == STACK_OK);
    *err = stack_error(stk);

    return;
}


void stack_pop(struct stack *const stk, stack_elem_t *out, ssize_t *err) {
    assert(stk != NULL);
    assert(err != NULL);

    assert(stack_error(stk) == STACK_OK);
    if ((*err = stack_error(stk)) != STACK_OK)
        return;

    ON_DEBUG(stk->err_struct->last_called = __func__;)

    ON_HASH_PROT(stack_update_hash(stk);)
    assert((*err = stack_error(stk)) == STACK_OK);
    if ((*err = stack_error(stk)) != STACK_OK)
        return;

    *out = stk->data[--stk->size ON_CANARY_PROT(+1)];

    ON_HASH_PROT(stack_update_hash(stk);)

    while (stk->capacity > 1 && stk->size * 4 < stk->capacity) {
        if ((*err = stack_shrink(stk)) != STACK_OK)
            return;
    }

    ON_HASH_PROT(stack_update_hash(stk);)
    assert(stack_error(stk) == STACK_OK);
    *err = stack_error(stk);

    return;
}


void stack_dump(const struct stack *const stk) {
    assert(stk != NULL);

    ON_DEBUG(
    if (stk->err_struct == NULL) {
        fprintf(stderr, "Stack dumping is not safe. Stack is probably uninitialized or destructed.\n");
        return;
    }
    )

    ON_HASH_PROT(
    if (stk->hash_struct == NULL) {
        fprintf(stderr, "Stack dumping is not safe. Stack is probably uninitialized or destructed.\n");
        return;
    }
    )

    fprintf(stderr, "\nDumping stack structure:\n");
    ON_DEBUG(fprintf(stderr, "Variable \"%s\" of type stack at [%p] created in %s:%lu called from function \"%s\":\n",
                    stk->err_struct->var_name, stk->err_struct->origin_ptr, stk->err_struct->origin_filename, stk->err_struct->line, stk->err_struct->last_called);)
    ON_DEBUG(if (stk->err_struct->error != STACK_OK) fprintf(stderr, RED "Error code %ld: %s\n" BASE_CLR,
                                                stk->err_struct->error, stk->err_struct->err_name);)

    ON_CANARY_PROT(fprintf(stderr, "\tleft  canary value: %lu\texpected canary: %lu\n", stk->left_canary, GET_CANARY(&stk->left_canary));)
    ON_CANARY_PROT(fprintf(stderr, "\tright canary value: %lu\texpected canary: %lu\n", stk->right_canary, GET_CANARY(&stk->right_canary));)
    ON_HASH_PROT(  fprintf(stderr, "\thash:               %lx\texpected hash  : %lx\n", stk->hash_struct->hash, stack_calculate_hash(stk));)
    ON_HASH_PROT(  fprintf(stderr, "\tdata hash:          %lx\texpected       : %lx\n", stk->hash_struct->data_hash, stack_calculate_data_hash(stk));)

    fprintf(stderr, "\tsize: %lu\n"
                    "\tcapacity: %lu\n"
                    "\tdata: [%p]\n"
    , stk->size, stk->capacity, stk->data
    );
    if (stk->data != NULL)
    {
        size_t max_size = stk->size < stk->capacity ? stk->capacity : stk->size;
        size_t min_size = stk->size > stk->capacity ? stk->capacity : stk->size;

        ON_CANARY_PROT(fprintf(stderr, "\t\t  data[%lu] = % " ELEM_SPEC ";\texpected canary = %" ELEM_SPEC "\n", 0LU, stk->data[0], GET_DATA_CANARY(stack_elem_t, &stk->data[0]));)
        for (size_t i = 0 ON_CANARY_PROT(+1); i < min_size ON_CANARY_PROT(+1); ++i) {
            fprintf(stderr, "\t\t* data[%lu] = % " ELEM_SPEC "\n", i, stk->data[i]);
        }

        for (size_t i = min_size ON_CANARY_PROT(+1); i < max_size ON_CANARY_PROT(+1); ++i) {
            fprintf(stderr, "\t\t  data[%lu] = % " ELEM_SPEC "\n", i, stk->data[i]);
        }
        ON_CANARY_PROT(fprintf(stderr, "\t\t  data[%lu] = % " ELEM_SPEC ";\texpected canary = %" ELEM_SPEC "\n", max_size + 1, stk->data[max_size + 1], GET_DATA_CANARY(stack_elem_t, &stk->data[max_size + 1]));)
    }

    fprintf(stderr, "\n\n");
}


bool doubles_equal(const long double x, const long double y) {
    const char *byte1 = (const char *) &x, *byte2 = (const char *) &y;

    for (size_t i = 0; i < 10; ++i) {
        if (*(byte1++) != (*(byte2++)))
            return 0;
    }
    return 1;
}


ON_HASH_PROT(
size_t stack_djb2_hash(const struct stack *const stk) {
    assert(stk != NULL);

    size_t hash = 5381;
    unsigned short idx = 0;

    while (idx < sizeof(*stk)) {
        hash = ((hash << 5) + hash) + (size_t) *( (const char *) stk + idx );
        idx++;
    }

    return hash;
}
)


ON_HASH_PROT(
size_t stack_djb2_data_hash(const struct stack *const stk) {
    if (stk->data == NULL)
        return 0;

    size_t hash = 5381;
    unsigned short idx = 0;

    while (idx < ((stk->size ON_CANARY_PROT(+2)) * sizeof(stack_elem_t))) {
        hash = ((hash << 5) + hash) + (size_t) *( (char *) stk->data + idx );
        idx++;
    }

    return hash;
}
)
