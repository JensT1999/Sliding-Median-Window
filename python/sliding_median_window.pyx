cdef extern from "medianwindow_api.h":
    bint sliding_medianwindow(double *inputArray, size_t length, size_t windowSize, size_t steps,
    bint ignoreNaNWindows, double *outputArray)

import numpy as np
cimport numpy as np

def sliding_median_window(np.ndarray[np.float64_t, ndim=1] array,
    windowSize, steps, ignoreNaNWindows,
    np.ndarray[np.float64_t, ndim=1] output):
    """
    Provides a Python interface for the sliding median window.

    Parameters
    ----------
    array : np.ndarray
        Input sequence as a 1D array of type float64.
    windowSize : int
        Size of the sliding window.
    steps : int
        Number of steps between median outputs.
    ignoreNaNWindows : bool
        true: If at least one NaN is present in the window, the resulting median for that window will be NaN;
        false: The median is calculated using only valid (non-NaN) elements inside the window.
        Infinite values (+/-INFINITY) are considered valid. If no valid elements remain in the window,
        the resulting median will be NaN. Example: valid_elements = windowSize - NaN_count
    output : np.ndarray
        Output array to store the computed medians.

    Returns
    -------
    bool
        True on success, False otherwise.
    """
    cdef Py_ssize_t len = array.size
    cdef np.ndarray[np.float64_t, ndim=1] c_array = np.ascontiguousarray(array, dtype=np.float64)
    cdef double* inputArrayPtr = <double*> c_array.data
    cdef np.ndarray[np.float64_t, ndim=1] c_output = np.ascontiguousarray(output, dtype=np.float64)
    cdef double* output_array = <double*> c_output.data

    return sliding_medianwindow(inputArrayPtr, len, windowSize, steps, ignoreNaNWindows, output_array)
