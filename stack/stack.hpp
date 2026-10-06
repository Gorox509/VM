#ifndef STACK_H
#define STACK_H

typedef double stack_elem_t;

#ifdef STACK_DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

#ifdef CANARY_PROT
#define ON_CANARY_PROT(...) __VA_ARGS__
#else
#define ON_CANARY_PROT(...)
#endif

#ifdef HASH_PROT
#define ON_HASH_PROT(...) __VA_ARGS__
#else
#define ON_HASH_PROT(...)
#endif

#ifndef STACK_DEPENDENCIES
#define STACK_DEPENDENCIES
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#endif


enum STACK_ERRORS {
    STACK_OK = 0,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    STACK_WRONG_DATA_PTR,
    STACK_WRONG_CAPACITY,

    ON_CANARY_PROT(
    STACK_WRONG_LEFT_CANARY,
    STACK_WRONG_RIGHT_CANARY,
    STACK_WRONG_DATA_LEFT_CANARY,
    STACK_WRONG_DATA_RIGHT_CANARY,
    )

    ON_HASH_PROT(
    STACK_WRONG_HASH,
    STACK_WRONG_DATA_HASH,
    )
};


struct stack {
    ON_CANARY_PROT(size_t left_canary = 0;)

    size_t size = 0;
    size_t capacity = 0;
    stack_elem_t *data = NULL;

    bool data_allocated = 0;

    ON_HASH_PROT(struct stack_hash *hash_struct;)

    ON_DEBUG(struct stack_err_struct *err_struct;)

    ON_CANARY_PROT(size_t right_canary = 0;)
};

ON_DEBUG(
struct stack_err_struct {
    const char *origin_filename;
    struct stack *origin_ptr;
    const char *var_name;
    size_t line;
    const char *last_called;
    ssize_t error;
    const char *err_name;
};
)

ON_HASH_PROT(
struct stack_hash {
    size_t hash = 0;
    size_t data_hash = 0;
};
)


ssize_t stack_error         (struct         stack *stk);

ON_HASH_PROT(size_t stack_calculate_hash
                            (const struct   stack *stk);)
ON_HASH_PROT(size_t stack_calculate_data_hash
                            (const struct   stack *stk);)
ON_HASH_PROT(void stack_update_hash
                            (struct         stack *stk);)


ON_CANARY_PROT(ssize_t stack_assign_data_canaries
                            (struct         stack *stk);)

ssize_t stack_constructor   (struct         stack *stk,       size_t       initial_size
                ON_DEBUG    (,const         char  *filename,  size_t       line,        const char *var_name));

ssize_t stack_destructor    (struct         stack *stk);
ssize_t stack_extend        (struct         stack *stk);
ssize_t stack_shrink        (struct         stack *stk);
void    stack_push          (struct         stack *stk,       stack_elem_t elem,        ssize_t    *err);
void    stack_pop           (struct         stack *stk,       stack_elem_t *out,        ssize_t    *err);
void    stack_dump          (const struct   stack *stk);


bool    doubles_equal       (const long double x,      const long double y);


ON_HASH_PROT(size_t stack_djb2_hash
                            (const struct   stack *stk);)
ON_HASH_PROT(size_t stack_djb2_data_hash
                            (const struct   stack *stk);)

#endif
