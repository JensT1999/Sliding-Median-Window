#include "medianwindow_api.h"
#include "median.h"
#include "config.h"

bool sliding_medianwindow(double *inputArray, size_t length, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, double *outputArray) {
    if(windowSize <= TINY_MEDIANWINDOW_THRESHOLD)
        return sliding_tiny_medianwindow(inputArray, length, windowSize, steps, ignoreNaNWindows, outputArray);

    return sliding_heap_medianwindow(inputArray, length, windowSize, steps, ignoreNaNWindows, outputArray);
}

bool sliding_big_window(double *inputArray, size_t length, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, double *outputArray) {
    return sliding_heap_medianwindow(inputArray, length, windowSize, steps, ignoreNaNWindows, outputArray);
}

bool sliding_tiny_window(double *inputArray, size_t length, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, double *outputArray) {
    return sliding_tiny_medianwindow(inputArray, length, windowSize, steps, ignoreNaNWindows, outputArray);
}
