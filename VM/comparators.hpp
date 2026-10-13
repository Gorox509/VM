#ifndef COMPARATORS_H
#define COMPARATORS_H

#include "../stack/stack.hpp"

int compare_always(         stack_elem_t *val1, stack_elem_t *val2);
int compare_above(          stack_elem_t *val1, stack_elem_t *val2);
int compare_above_or_equal( stack_elem_t *val1, stack_elem_t *val2);
int compare_below(          stack_elem_t *val1, stack_elem_t *val2);
int compare_below_or_equal( stack_elem_t *val1, stack_elem_t *val2);
int compare_equal(          stack_elem_t *val1, stack_elem_t *val2);
int compare_not_equal(      stack_elem_t *val1, stack_elem_t *val2);

#endif
