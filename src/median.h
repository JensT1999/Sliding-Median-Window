#ifndef MEDIAN_H
#define MEDIAN_H

#include <stdbool.h>
#include <stdlib.h>
#include "tiny_medianwindow.h"
#include "median_window.h"
#include "window_helpers.h"

bool sliding_heap_medianwindow(double *restrict array, size_t length, size_t windowSize, size_t steps,
    double *restrict result);

bool sliding_tiny_medianwindow(double *restrict array, size_t length, size_t windowSize,
    size_t steps, double *restrict result);

#endif
