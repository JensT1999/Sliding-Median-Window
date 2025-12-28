/**
 * @file tiny_medianwindow.c
 * @author Jens Trappmann (jens.trappmann@icloud.com)
 * @brief This file implements median sorting networks for window sizes 2 to 8,
 *        which are applied in sliding window operations where the window is small enough
 *        that sorting networks are more efficient than heap-based methods.
 * @version 0.1
 * @date 2025-12-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "tiny_medianwindow.h"

static void set_sort_and_calc_function(Tiny_MedianWindow *window);
static void sort_and_calc_median2(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median3(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median4(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median5(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median6(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median7(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median8(double *restrict inputStartPtr, double *restrict result);
static inline void values_swap(double *restrict a, double *restrict b);
static inline double value_get(double *restrict value);

void tiny_medianwindow_initialize(char **memory, size_t windowSize,
    size_t steps, Tiny_MedianWindow **window) {
    Tiny_MedianWindow *targetWindow = (Tiny_MedianWindow* ) __builtin_assume_aligned(*memory, STD_ALIGNMENT);
    targetWindow->windowSize = windowSize;
    targetWindow->steps = steps;
    targetWindow->stepDistance = 0;
    targetWindow->tailPtr = 0;
    targetWindow->headPtr = 0;
    set_sort_and_calc_function(targetWindow);
    *window = targetWindow;
}

void tiny_medianwindow_move_head(Tiny_MedianWindow *restrict window) {
    window->headPtr += 1;
}

void tiny_medianwindow_move_tail(Tiny_MedianWindow *restrict window) {
    window->tailPtr += 1;
}

void tiny_medianwindow_result(Tiny_MedianWindow *restrict window, double *restrict input, double *restrict output) {
    window->sort_and_calc_median((input + window->tailPtr), output);
}

static void set_sort_and_calc_function(Tiny_MedianWindow *window) {
    const size_t windowSize = window->windowSize;
    switch (windowSize) {
    case 2:
        window->sort_and_calc_median = &sort_and_calc_median2;
        break;
    case 3:
        window->sort_and_calc_median = &sort_and_calc_median3;
        break;
    case 4:
        window->sort_and_calc_median = &sort_and_calc_median4;
        break;
    case 5:
        window->sort_and_calc_median = &sort_and_calc_median5;
        break;
    case 6:
        window->sort_and_calc_median = &sort_and_calc_median6;
        break;
    case 7:
        window->sort_and_calc_median = &sort_and_calc_median7;
        break;
    case 8:
        window->sort_and_calc_median = &sort_and_calc_median8;
        break;
    default: break;
    }
}

static void sort_and_calc_median2(double *restrict inputStartPtr, double *restrict result) {
    double value0 = value_get(inputStartPtr++);
    double value1 = value_get(inputStartPtr);

    if(value0 > value1) values_swap(&value0, &value1);

    *result = ((value0 + value1) / 2);
}

static void sort_and_calc_median3(double *restrict inputStartPtr, double *restrict result) {
    double value0 = value_get(inputStartPtr++);
    double value1 = value_get(inputStartPtr++);
    double value2 = value_get(inputStartPtr);

    if(value0 > value1) values_swap(&value0, &value1);
    if(value1 > value2) values_swap(&value1, &value2);
    if(value0 > value1) values_swap(&value0, &value1);

    *result = value1;
}

static void sort_and_calc_median4(double *restrict inputStartPtr, double *restrict result) {
    double value0 = value_get(inputStartPtr++);
    double value1 = value_get(inputStartPtr++);
    double value2 = value_get(inputStartPtr++);
    double value3 = value_get(inputStartPtr);

    if(value0 > value1) values_swap(&value0, &value1);
    if(value2 > value3) values_swap(&value2, &value3);
    if(value0 > value2) values_swap(&value0, &value2);
    if(value1 > value3) values_swap(&value1, &value3);

    *result = ((value1 + value2) / 2);
}

static void sort_and_calc_median5(double *restrict inputStartPtr, double *restrict result) {
    double value0 = value_get(inputStartPtr++);
    double value1 = value_get(inputStartPtr++);
    double value2 = value_get(inputStartPtr++);
    double value3 = value_get(inputStartPtr++);
    double value4 = value_get(inputStartPtr);

    if(value0 > value1) values_swap(&value0, &value1);
    if(value2 > value3) values_swap(&value2, &value3);
    if(value0 > value2) values_swap(&value0, &value2);
    if(value1 > value3) values_swap(&value1, &value3);
    if(value2 > value4) values_swap(&value2, &value4);
    if(value1 > value2) values_swap(&value1, &value2);
    if(value2 > value4) values_swap(&value2, &value4);

    *result = value2;
}

static void sort_and_calc_median6(double *restrict inputStartPtr, double *restrict result) {
    double value0 = value_get(inputStartPtr++);
    double value1 = value_get(inputStartPtr++);
    double value2 = value_get(inputStartPtr++);
    double value3 = value_get(inputStartPtr++);
    double value4 = value_get(inputStartPtr++);
    double value5 = value_get(inputStartPtr);

    if(value0 > value1) values_swap(&value0, &value1);
    if(value4 > value5) values_swap(&value4, &value5);
    if(value0 > value5) values_swap(&value0, &value5);
    if(value1 > value3) values_swap(&value1, &value3);
    if(value2 > value4) values_swap(&value2, &value4);
    if(value0 > value2) values_swap(&value0, &value2);
    if(value1 > value4) values_swap(&value1, &value4);
    if(value3 > value5) values_swap(&value3, &value5);
    if(value1 > value2) values_swap(&value1, &value2);
    if(value3 > value4) values_swap(&value3, &value4);

    *result = ((value2 + value3) / 2);
}

static void sort_and_calc_median7(double *restrict inputStartPtr, double *restrict result) {
    double value0 = value_get(inputStartPtr++);
    double value1 = value_get(inputStartPtr++);
    double value2 = value_get(inputStartPtr++);
    double value3 = value_get(inputStartPtr++);
    double value4 = value_get(inputStartPtr++);
    double value5 = value_get(inputStartPtr++);
    double value6 = value_get(inputStartPtr);

    if(value0 > value6) values_swap(&value0, &value6);
    if(value1 > value2) values_swap(&value1, &value2);
    if(value3 > value4) values_swap(&value3, &value4);
    if(value0 > value2) values_swap(&value0, &value2);
    if(value1 > value4) values_swap(&value1, &value4);
    if(value3 > value5) values_swap(&value3, &value5);
    if(value0 > value1) values_swap(&value0, &value1);
    if(value2 > value5) values_swap(&value2, &value5);
    if(value4 > value6) values_swap(&value4, &value6);
    if(value1 > value3) values_swap(&value1, &value3);
    if(value2 > value4) values_swap(&value2, &value4);
    if(value3 > value4) values_swap(&value3, &value4);
    if(value2 > value3) values_swap(&value2, &value3);

    *result = value3;
}

static void sort_and_calc_median8(double *restrict inputStartPtr, double *restrict result) {
    double value0 = value_get(inputStartPtr++);
    double value1 = value_get(inputStartPtr++);
    double value2 = value_get(inputStartPtr++);
    double value3 = value_get(inputStartPtr++);
    double value4 = value_get(inputStartPtr++);
    double value5 = value_get(inputStartPtr++);
    double value6 = value_get(inputStartPtr++);
    double value7 = value_get(inputStartPtr);

    if(value0 > value2) values_swap(&value0, &value2);
    if(value1 > value3) values_swap(&value1, &value3);
    if(value4 > value6) values_swap(&value4, &value6);
    if(value5 > value7) values_swap(&value5, &value7);
    if(value0 > value4) values_swap(&value0, &value4);
    if(value1 > value5) values_swap(&value1, &value5);
    if(value2 > value6) values_swap(&value2, &value6);
    if(value3 > value7) values_swap(&value3, &value7);
    if(value0 > value1) values_swap(&value0, &value1);
    if(value2 > value4) values_swap(&value2, &value4);
    if(value3 > value5) values_swap(&value3, &value5);
    if(value6 > value7) values_swap(&value6, &value7);
    if(value2 > value3) values_swap(&value2, &value3);
    if(value4 > value5) values_swap(&value4, &value5);
    if(value1 > value4) values_swap(&value1, &value4);
    if(value3 > value6) values_swap(&value3, &value6);

    *result = ((value3 + value4) / 2);
}

static inline void values_swap(double *restrict a, double *restrict b) {
    const double tempValue = *b;
    *b = *a;
    *a = tempValue;
}

static inline double value_get(double *restrict value) {
    const double v = *value;
    return (!isfinite(v)) ? DBL_MAX : v;
}
