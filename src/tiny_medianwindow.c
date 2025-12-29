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

#include <stdio.h>

static void set_sort_and_calc_function(Tiny_MedianWindow *window, bool ignoreNaNWindows);
static void sort_and_calc_median2(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median2_nan_handle(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median3(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median3_nan_handle(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median4(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median4_nan_handle(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median5(double *restrict inputArray, double *restrict result);
static void sort_and_calc_median5_nan_handle(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median6(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median6_nan_handle(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median7(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median7_nan_handle(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median8(double *restrict inputStartPtr, double *restrict result);
static void sort_and_calc_median8_nan_handle(double *restrict inputStartPtr, double *restrict result);
static inline void values_swap(double *restrict a, double *restrict b);

static inline void values_build_nan_array(double *restrict values, size_t length, bool *restrict nanArray,
    size_t *nanCount);
static inline void values_translate_nan_array(double *restrict values, size_t length, bool *restrict nanArray,
    double *validValues);

static inline void median_network_select(size_t validNum, double *restrict values);
static inline void median_network_2(double *restrict values);
static inline void median_network_3(double *restrict values);
static inline void median_network_4(double *restrict values);
static inline void median_network_5(double *restrict values);
static inline void median_network_6(double *restrict values);
static inline void median_network_7(double *restrict values);
static inline void median_network_8(double *restrict values);

void tiny_medianwindow_initialize(char **memory, size_t windowSize,
    size_t steps, bool ignoreNaNWindows, Tiny_MedianWindow **window) {
    Tiny_MedianWindow *targetWindow = (Tiny_MedianWindow* ) __builtin_assume_aligned(*memory, STD_ALIGNMENT);
    targetWindow->windowSize = windowSize;
    targetWindow->steps = steps;
    targetWindow->stepDistance = 0;
    targetWindow->tailPtr = 0;
    targetWindow->headPtr = 0;
    set_sort_and_calc_function(targetWindow, ignoreNaNWindows);
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

static void set_sort_and_calc_function(Tiny_MedianWindow *window, bool ignoreNaNWindows) {
    const size_t windowSize = window->windowSize;
    switch (windowSize) {
        case 2:
            if(ignoreNaNWindows)
                window->sort_and_calc_median = &sort_and_calc_median2_nan_handle;
            else
                window->sort_and_calc_median = &sort_and_calc_median2;
            break;
        case 3:
            if(ignoreNaNWindows)
                window->sort_and_calc_median = &sort_and_calc_median3_nan_handle;
            else
                window->sort_and_calc_median = &sort_and_calc_median3;
            break;
        case 4:
            if(ignoreNaNWindows)
                window->sort_and_calc_median = &sort_and_calc_median4_nan_handle;
            else
                window->sort_and_calc_median = &sort_and_calc_median4;
            break;
        case 5: {
            if(ignoreNaNWindows)
                window->sort_and_calc_median = &sort_and_calc_median5_nan_handle;
            else
                window->sort_and_calc_median = &sort_and_calc_median5;
            break;
        }
        case 6:
            if(ignoreNaNWindows)
                window->sort_and_calc_median = &sort_and_calc_median6_nan_handle;
            else
                window->sort_and_calc_median = &sort_and_calc_median6;
            break;
        case 7:
            if(ignoreNaNWindows)
                window->sort_and_calc_median = &sort_and_calc_median7_nan_handle;
            else
                window->sort_and_calc_median = &sort_and_calc_median7;
            break;
        case 8:
            if(ignoreNaNWindows)
                window->sort_and_calc_median = &sort_and_calc_median8_nan_handle;
            else
                window->sort_and_calc_median = &sort_and_calc_median8;
            break;
    }
}

#define WINDOW_SIZE_2 2

static void sort_and_calc_median2(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_2];
    values[0] = *(inputStartPtr++);
    values[1] = *(inputStartPtr++);

    bool nanArray[WINDOW_SIZE_2] = {false, false};
    size_t nanCount = 0;
    values_build_nan_array(values, WINDOW_SIZE_2, nanArray, &nanCount);
    if(nanCount == 0) {
        median_network_2(values);
        *result = ((values[0] + values[1]) / 2);
        return;
    } else if(nanCount == WINDOW_SIZE_2) {
        *result = NAN;
        return;
    }

    double validValues[WINDOW_SIZE_2];
    values_translate_nan_array(values, WINDOW_SIZE_2, nanArray, validValues);
    *result = validValues[0];
}

static void sort_and_calc_median2_nan_handle(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_2];
    values[0] = *(inputStartPtr++);
    values[1] = *(inputStartPtr++);

    if((isnan(values[0]) || (isnan(values[1])))) {
        *result = NAN;
        return;
    }

    median_network_2(values);
    *result = ((values[0] + values[1]) / 2);
}

#define WINDOW_SIZE_3 3

static void sort_and_calc_median3(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_3];
    for(size_t i = 0; i < WINDOW_SIZE_3; i++)
        values[i] = *(inputStartPtr++);

    bool nanArray[WINDOW_SIZE_3] = {false, false, false};
    size_t nanCount = 0;
    values_build_nan_array(values, WINDOW_SIZE_3, nanArray, &nanCount);
    if(nanCount == 0) {
        median_network_3(values);
        *result = values[1];
        return;
    } else if(nanCount == WINDOW_SIZE_3) {
        *result = NAN;
        return;
    }

    double validValues[WINDOW_SIZE_3];
    values_translate_nan_array(values, WINDOW_SIZE_3, nanArray, validValues);
    const size_t validNum = (WINDOW_SIZE_3 - nanCount);
    if(validNum > 1)
        median_network_2(validValues);

    const size_t middle = (validNum / 2);
    *result = ((validNum % 2) == 0) ?
        ((validValues[middle - 1] + validValues[middle]) / 2) :
        ((validValues[middle]));
}

static void sort_and_calc_median3_nan_handle(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_3];
    for(size_t i = 0; i < WINDOW_SIZE_3; i++)
        values[i] = *(inputStartPtr++);

    if((isnan(values[0])) || (isnan(values[1])) || (isnan(values[2]))) {
        *result = NAN;
        return;
    }

    median_network_3(values);
    *result = values[1];
}

#define WINDOW_SIZE_4 4

static void sort_and_calc_median4(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_4];
    for(size_t i = 0; i < WINDOW_SIZE_4; i++)
        values[i] = *(inputStartPtr++);

    bool nanArray[WINDOW_SIZE_4] = {false, false, false, false};
    size_t nanCount = 0;
    values_build_nan_array(values, WINDOW_SIZE_4, nanArray, &nanCount);
    if(nanCount == 0) {
        median_network_4(values);
        *result = ((values[1] + values[2]) / 2);
        return;
    } else if(nanCount == WINDOW_SIZE_4) {
        *result = NAN;
        return;
    }

    double validValues[WINDOW_SIZE_4];
    values_translate_nan_array(values, WINDOW_SIZE_4, nanArray, validValues);
    const size_t validNum = (WINDOW_SIZE_4 - nanCount);
    if(validNum > 1)
        median_network_select(validNum, validValues);

    const size_t middle = (validNum / 2);
    *result = ((validNum % 2) == 0) ?
        ((validValues[middle - 1] + validValues[middle]) / 2) :
        ((validValues[middle]));
}

static void sort_and_calc_median4_nan_handle(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_4];
    for(size_t i = 0; i < WINDOW_SIZE_4; i++)
        values[i] = *(inputStartPtr++);

    if((isnan(values[0])) || (isnan(values[1])) || (isnan(values[2])) || (isnan(values[3]))) {
        *result = NAN;
        return;
    }

    median_network_4(values);
    *result = ((values[1] + values[2]) / 2);
}

#define WINDOW_SIZE_5 5

static void sort_and_calc_median5(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_5];
    for(size_t i = 0; i < WINDOW_SIZE_5; i++)
        values[i] = *(inputStartPtr++);

    bool nanArray[WINDOW_SIZE_5] = {false, false, false, false, false};
    size_t nanCount = 0;
    values_build_nan_array(values, WINDOW_SIZE_5, nanArray, &nanCount);
    if(nanCount == 0) {
        median_network_5(values);
        *result = values[2];
        return;
    } else if(nanCount == WINDOW_SIZE_5) {
        *result = NAN;
        return;
    }

    double validValues[WINDOW_SIZE_5];
    values_translate_nan_array(values, WINDOW_SIZE_5, nanArray, validValues);
    const size_t validNum = (WINDOW_SIZE_5 - nanCount);
    if(validNum > 1)
        median_network_select(validNum, validValues);

    const size_t middle = (validNum / 2);
    *result = ((validNum % 2) == 0) ?
        ((validValues[middle - 1] + validValues[middle]) / 2) :
        ((validValues[middle]));
}

static void sort_and_calc_median5_nan_handle(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_5];
    for(size_t i = 0; i < WINDOW_SIZE_5; i++)
        values[i] = *(inputStartPtr++);

    if((isnan(values[0])) || (isnan(values[1])) || (isnan(values[2])) ||
        (isnan(values[3])) || (isnan(values[4]))) {
        *result = NAN;
        return;
    }

    median_network_5(values);
    *result = values[2];
}

#define WINDOW_SIZE_6 6

static void sort_and_calc_median6(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_6];
    for(size_t i = 0; i < WINDOW_SIZE_6; i++)
        values[i] = *(inputStartPtr++);

    bool nanArray[WINDOW_SIZE_6] = {false, false, false, false, false, false};
    size_t nanCount = 0;
    values_build_nan_array(values, WINDOW_SIZE_6, nanArray, &nanCount);
    if(nanCount == 0) {
        median_network_6(values);
        *result = ((values[2] + values[3]) / 2);
        return;
    } else if(nanCount == WINDOW_SIZE_6) {
        *result = NAN;
        return;
    }

    double validValues[WINDOW_SIZE_6];
    values_translate_nan_array(values, WINDOW_SIZE_6, nanArray, validValues);
    const size_t validNum = (WINDOW_SIZE_6 - nanCount);
    if(validNum > 1)
        median_network_select(validNum, validValues);

    const size_t middle = (validNum / 2);
    *result = ((validNum % 2) == 0) ?
        ((validValues[middle - 1] + validValues[middle]) / 2) :
        ((validValues[middle]));
}

static void sort_and_calc_median6_nan_handle(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_6];
    for(size_t i = 0; i < WINDOW_SIZE_6; i++)
        values[i] = *(inputStartPtr++);

    if((isnan(values[0])) || (isnan(values[1])) || (isnan(values[2])) ||
        (isnan(values[3])) || (isnan(values[4])) || (isnan(values[5]))) {
        *result = NAN;
        return;
    }

    median_network_6(values);
    *result = ((values[2] + values[3]) / 2);
}

#define WINDOW_SIZE_7 7

static void sort_and_calc_median7(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_7];
    for(size_t i = 0; i < WINDOW_SIZE_7; i++)
        values[i] = *(inputStartPtr++);

    bool nanArray[WINDOW_SIZE_7] = {false, false, false, false, false, false, false};
    size_t nanCount = 0;
    values_build_nan_array(values, WINDOW_SIZE_7, nanArray, &nanCount);
    if(nanCount == 0) {
        median_network_7(values);
        *result = values[3];
        return;
    } else if(nanCount == WINDOW_SIZE_7) {
        *result = NAN;
        return;
    }

    double validValues[WINDOW_SIZE_7];
    values_translate_nan_array(values, WINDOW_SIZE_7, nanArray, validValues);
    const size_t validNum = (WINDOW_SIZE_7 - nanCount);
    if(validNum > 1)
        median_network_select(validNum, validValues);

    const size_t middle = (validNum / 2);
    *result = ((validNum % 2) == 0) ?
        ((validValues[middle - 1] + validValues[middle]) / 2) :
        ((validValues[middle]));
}

static void sort_and_calc_median7_nan_handle(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_7];
    for(size_t i = 0; i < WINDOW_SIZE_7; i++)
        values[i] = *(inputStartPtr++);

    if((isnan(values[0])) || (isnan(values[1])) || (isnan(values[2])) ||
        (isnan(values[3])) || (isnan(values[4])) || (isnan(values[5])) ||
        (isnan(values[6]))) {
        *result = NAN;
        return;
    }

    median_network_7(values);
    *result = values[3];
}

#define WINDOW_SIZE_8 8

static void sort_and_calc_median8(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_8];
    for(size_t i = 0; i < WINDOW_SIZE_8; i++)
        values[i] = *(inputStartPtr++);

    bool nanArray[WINDOW_SIZE_8] = {false, false, false, false, false, false, false, false};
    size_t nanCount = 0;
    values_build_nan_array(values, WINDOW_SIZE_8, nanArray, &nanCount);
    if(nanCount == 0) {
        median_network_8(values);
        *result = ((values[3] + values[4]) / 2);
        return;
    } else if(nanCount == WINDOW_SIZE_8) {
        *result = NAN;
        return;
    }

    double validValues[WINDOW_SIZE_8];
    values_translate_nan_array(values, WINDOW_SIZE_8, nanArray, validValues);
    const size_t validNum = (WINDOW_SIZE_8 - nanCount);
    if(validNum > 1)
        median_network_select(validNum, validValues);

    const size_t middle = (validNum / 2);
    *result = ((validNum % 2) == 0) ?
        ((validValues[middle - 1] + validValues[middle]) / 2) :
        ((validValues[middle]));
}

static void sort_and_calc_median8_nan_handle(double *restrict inputStartPtr, double *restrict result) {
    double values[WINDOW_SIZE_8];
    for(size_t i = 0; i < WINDOW_SIZE_8; i++)
        values[i] = *(inputStartPtr++);

    if((isnan(values[0])) || (isnan(values[1])) || (isnan(values[2])) ||
        (isnan(values[3])) || (isnan(values[4])) || (isnan(values[5])) ||
        (isnan(values[6])) || (isnan(values[7]))) {
        *result = NAN;
        return;
    }

    median_network_8(values);
    *result = ((values[3] + values[4]) / 2);
}

static inline void values_swap(double *restrict a, double *restrict b) {
    const double tempValue = *b;
    *b = *a;
    *a = tempValue;
}

static inline void values_build_nan_array(double *restrict values, size_t length, bool *restrict nanArray,
    size_t *nanCount) {
    size_t nanResultCount = 0;
    for(size_t i = 0; i < length; i++) {
        const bool isNaN = isnan(values[i]);
        nanArray[i] = isNaN;
        nanResultCount += isNaN;
    }

    *nanCount = nanResultCount;
}

static inline void values_translate_nan_array(double *restrict values, size_t length, bool *restrict nanArray,
    double *validValues) {
    size_t insertPosition = 0;
    for(size_t i = 0; i < length; i++) {
        const bool valid = (!nanArray[i]);
        if(!valid)
            continue;

        validValues[insertPosition] = values[i];
        insertPosition++;
    }
}

static inline void median_network_select(size_t validNum, double *restrict values) {
    switch (validNum) {
        case 2:
            median_network_2(values);
            break;
        case 3:
            median_network_3(values);
            break;
        case 4:
            median_network_4(values);
            break;
        case 5:
            median_network_5(values);
            break;
        case 6:
            median_network_6(values);
            break;
        case 7:
            median_network_7(values);
            break;
        case 8:
            median_network_8(values);
    }
}

static inline void median_network_2(double *restrict values) {
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
}

static inline void median_network_3(double *restrict values) {
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
    if(values[1] > values[2]) values_swap(&values[1], &values[2]);
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
}

static inline void median_network_4(double *restrict values) {
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
    if(values[2] > values[3]) values_swap(&values[2], &values[3]);
    if(values[0] > values[2]) values_swap(&values[0], &values[2]);
    if(values[1] > values[3]) values_swap(&values[1], &values[3]);
}

static inline void median_network_5(double *restrict values) {
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
    if(values[2] > values[3]) values_swap(&values[2], &values[3]);
    if(values[0] > values[2]) values_swap(&values[0], &values[2]);
    if(values[1] > values[3]) values_swap(&values[1], &values[3]);
    if(values[2] > values[4]) values_swap(&values[2], &values[4]);
    if(values[1] > values[2]) values_swap(&values[1], &values[2]);
    if(values[2] > values[4]) values_swap(&values[2], &values[4]);
}

static inline void median_network_6(double *restrict values) {
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
    if(values[4] > values[5]) values_swap(&values[4], &values[5]);
    if(values[0] > values[5]) values_swap(&values[0], &values[5]);
    if(values[1] > values[3]) values_swap(&values[1], &values[3]);
    if(values[2] > values[4]) values_swap(&values[2], &values[4]);
    if(values[0] > values[2]) values_swap(&values[0], &values[2]);
    if(values[1] > values[4]) values_swap(&values[1], &values[4]);
    if(values[3] > values[5]) values_swap(&values[3], &values[5]);
    if(values[1] > values[2]) values_swap(&values[1], &values[2]);
    if(values[3] > values[4]) values_swap(&values[3], &values[4]);
}

static inline void median_network_7(double *restrict values) {
    if(values[0] > values[6]) values_swap(&values[0], &values[6]);
    if(values[1] > values[2]) values_swap(&values[1], &values[2]);
    if(values[3] > values[4]) values_swap(&values[3], &values[4]);
    if(values[0] > values[2]) values_swap(&values[0], &values[2]);
    if(values[1] > values[4]) values_swap(&values[1], &values[4]);
    if(values[3] > values[5]) values_swap(&values[3], &values[5]);
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
    if(values[2] > values[5]) values_swap(&values[2], &values[5]);
    if(values[4] > values[6]) values_swap(&values[4], &values[6]);
    if(values[1] > values[3]) values_swap(&values[1], &values[3]);
    if(values[2] > values[4]) values_swap(&values[2], &values[4]);
    if(values[3] > values[4]) values_swap(&values[3], &values[4]);
    if(values[2] > values[3]) values_swap(&values[2], &values[3]);
}

static inline void median_network_8(double *restrict values) {
    if(values[0] > values[2]) values_swap(&values[0], &values[2]);
    if(values[1] > values[3]) values_swap(&values[1], &values[3]);
    if(values[4] > values[6]) values_swap(&values[4], &values[6]);
    if(values[5] > values[7]) values_swap(&values[5], &values[7]);
    if(values[0] > values[4]) values_swap(&values[0], &values[4]);
    if(values[1] > values[5]) values_swap(&values[1], &values[5]);
    if(values[2] > values[6]) values_swap(&values[2], &values[6]);
    if(values[3] > values[7]) values_swap(&values[3], &values[7]);
    if(values[0] > values[1]) values_swap(&values[0], &values[1]);
    if(values[2] > values[4]) values_swap(&values[2], &values[4]);
    if(values[3] > values[5]) values_swap(&values[3], &values[5]);
    if(values[6] > values[7]) values_swap(&values[6], &values[7]);
    if(values[2] > values[3]) values_swap(&values[2], &values[3]);
    if(values[4] > values[5]) values_swap(&values[4], &values[5]);
    if(values[1] > values[4]) values_swap(&values[1], &values[4]);
    if(values[3] > values[6]) values_swap(&values[3], &values[6]);
}
