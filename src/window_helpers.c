#include "window_helpers.h"

bool valid_window(double *array, size_t length, size_t windowSize, size_t steps, double *result) {
    if((array == NULL) || (length == 0) || (result == NULL))
        return false;

    if((windowSize <= 1) || (steps == 0))
        return false;

    return true;
}

bool resultarray_init(size_t inputLength, size_t windowSize, size_t steps,
    size_t *resultLength, double **result) {
    if((inputLength == 0) || (windowSize <= 1) || (steps == 0) || (result == NULL) || (*result != NULL))
        return false;

    const size_t estimatedLength = (inputLength - windowSize) / steps + 1;
    *result = (double* ) malloc(estimatedLength * SIZE_OF_DOUBLE);
    if(*result == NULL)
        return false;

    *resultLength = estimatedLength;
    return true;
}
