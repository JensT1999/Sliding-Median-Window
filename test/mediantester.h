#ifndef MEDIAN_TESTER_H
#define MEDIAN_TESTER_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void median_tester_gen_medians(double *array, size_t length, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, double *output);
static void build_nan_free_array(double *inputStartPtr, size_t length, double *output, size_t *nanCounter);
static int compare_doubles(const void *a, const void *b);

void median_tester_gen_medians(double *array, size_t length, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, double *output) {
    size_t currentPos = 0;
    const size_t neededWindows = ((length - windowSize) / steps + 1);
    const size_t copiedBytes = (windowSize * sizeof(double));
    double *buffer = (double* ) malloc(copiedBytes);
    for(size_t i = 0; i < neededWindows; i++) {
        size_t nanCounter = 0;
        build_nan_free_array((array + currentPos), windowSize, buffer, &nanCounter);
        if((nanCounter > 0) && (ignoreNaNWindows)) {
            *output = NAN;
            output++;
            currentPos += steps;
            continue;
        }

        const size_t validNum = (windowSize - nanCounter);
        qsort(buffer, validNum, sizeof(double), &compare_doubles);

        const size_t middle = (validNum / 2);
        if((validNum % 2) == 0) {
            *output = ((buffer[middle - 1] + buffer[middle]) / 2);
        } else {
            *output = buffer[middle];
        }

        output++;
        currentPos += steps;
    }

    free(buffer);
    buffer = NULL;
}

static void build_nan_free_array(double *inputStartPtr, size_t length, double *output, size_t *nanCounter) {
    size_t inputPosition = 0;
    for(size_t i = 0; i < length; i++) {
        const double v = *(inputStartPtr++);
        const bool isNaN = isnan(v);
        output[inputPosition] = v;
        inputPosition += (!isNaN);
        *nanCounter += isNaN;
    }
}

static int compare_doubles(const void *a, const void *b) {
    const double d1 = *((double* ) a);
    const double d2 = *((double* ) b);
    return ((d1 < d2) ? -1 : (d1 > d2) ? 1 : 0);
}

#endif
