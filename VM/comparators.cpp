#include "comparators.hpp"
#include "assert.h"

int compare_always(stack_elem_t *val1, stack_elem_t *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);
    return 1;
}

int compare_above(stack_elem_t *val1, stack_elem_t *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);
    return *val1 > *val2;
}

int compare_above_or_equal(stack_elem_t *val1, stack_elem_t *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);
    return *val1 >= *val2;
}

int compare_below(stack_elem_t *val1, stack_elem_t *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);
    return *val1 < *val2;
}

int compare_below_or_equal(stack_elem_t *val1, stack_elem_t *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);
    return *val1 <= *val2;
}

int compare_equal(stack_elem_t *val1, stack_elem_t *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);
    return *val1 == *val2;
}

int compare_not_equal(stack_elem_t *val1, stack_elem_t *val2) {
    assert(val1 != NULL);
    assert(val2 != NULL);
    return *val1 != *val2;
}
