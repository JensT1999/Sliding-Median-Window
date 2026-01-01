#ifndef MEDIANWINDOW_API_H
#define MEDIANWINDOW_API_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief This function provides the interface for the sliding median.
 * Important: The interface determines, depending on the size of the window, which strategy is applied to process it.
 * If the window size is below the TINY_MEDIANWINDOW_THRESHOLD, median networks are used.
 * Above the threshold, a double-heap approach, commonly applied for sliding median windows, is used.
 * Important: The specified steps only indicate when a median should be obtained.
 * Regardless of the step size, all elements of the input array pass through the window.
 * @param inputArray - the input sequence
 * @param length - the length/size of the input sequence
 * @param windowSize - the size of the window
 * @param steps - the steps that must be taken to obtain a median#
 * @param ignoreNaNWindows - true: If at least one NaN is present in the window, the resulting median
 *      for that window will be NaN; false: The median is calculated using only valid (non-NaN) elements inside
 *      the window. Infinite values (+/-INFINITY) are considered valid. If no valid elements remain in the window,
 *      the resulting median will be NaN. Example: valid_elements = windowSize - NaN_count
 * @param outputArray - the output sequence
 * @return - true on success; otherwise false
 */
bool sliding_medianwindow(double *inputArray, size_t length, size_t windowSize, size_t steps,
    bool ignoreNaNWindows, double *outputArray);

#endif
