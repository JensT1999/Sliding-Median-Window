#ifndef WINDOW_HELPERS_H
#define WINDOW_HELPERS_H

#include <stdbool.h>
#include <stdlib.h>
#include "config.h"

bool valid_window(double  *array, size_t length, size_t windowSize, size_t steps, double *result);

bool resultarray_init(size_t inputLength, size_t windowSize, size_t steps,
    size_t *resultLength, double **result);

#endif
