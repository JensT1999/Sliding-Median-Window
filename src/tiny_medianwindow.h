#ifndef TINY_MEDIANWINDOW_H
#define TINY_MEDIANWINDOW_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "config.h"

typedef struct Tiny_MedianWindow
{
    size_t windowSize;
    size_t steps;
    size_t stepDistance;
    size_t tailPtr;
    size_t headPtr;
    void (*sort_and_calc_median) (double *restrict, double *restrict);
} Tiny_MedianWindow;

void tiny_medianwindow_initialize(char **memory, size_t windowSize, size_t steps, Tiny_MedianWindow **window);
void tiny_medianwindow_move_head(Tiny_MedianWindow *restrict window);
void tiny_medianwindow_move_tail(Tiny_MedianWindow *restrict window);
void tiny_medianwindow_result(Tiny_MedianWindow *restrict window, double *restrict input, double *restrict output);

#define SIZE_OF_TINY_MEDIAN_WINDOW sizeof(Tiny_MedianWindow)

#endif
