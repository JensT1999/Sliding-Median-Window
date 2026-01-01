#include "medianwindow_api.h"
#include "median.h"

#define TINY_MEDIANWINDOW_THRESHOLD 8

bool sliding_medianwindow(double *inputArray, size_t length, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, double *outputArray) {
    if(windowSize <= TINY_MEDIANWINDOW_THRESHOLD)
        return sliding_tiny_medianwindow(inputArray, length, windowSize, steps, ignoreNaNWindows, outputArray);

    return sliding_heap_medianwindow(inputArray, length, windowSize, steps, ignoreNaNWindows, outputArray);
}
