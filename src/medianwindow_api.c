#include "medianwindow_api.h"
#include "median.h"
#include "config.h"

bool sliding_medianwindow(double *inputArray, size_t length, size_t windowSize, size_t steps, double *outputArray) {
    if(windowSize <= TINY_MEDIANWINDOW_THRESHOLD)
        return sliding_tiny_medianwindow(inputArray, length, windowSize, steps, outputArray);

    return sliding_heap_medianwindow(inputArray, length, windowSize, steps, outputArray);
}
