#include "median.h"

static inline bool median_window_full(MedianWindow *window);
static inline bool median_window_steps_reached(MedianWindow *window);
static inline bool tiny_medianwindow_full(Tiny_MedianWindow *window);
static inline bool tiny_medianwindow_steps_reached(Tiny_MedianWindow *window);

bool sliding_heap_medianwindow(double *restrict array, size_t length, size_t windowSize, size_t steps,
    double *restrict result) {
    if(!valid_window(array, length, windowSize, steps, result))
        return false;

    const size_t neededWindowMemory = medianwindow_est_mem(windowSize);
    char *memory = (char* ) malloc(neededWindowMemory);
    if(memory == NULL)
        return false;

    char *startMemPtr = memory;
    MedianWindow *window;
    medianwindow_initialize(&memory, windowSize, steps, &window);

    for(size_t i = 0; i < length; i++) {
        if(median_window_full(window)) {
            medianwindow_updateOld(window, array[i]);
            if(median_window_steps_reached(window)) {
                medianwindow_result(window, result);
                result++;
            }
        } else {
            medianwindow_addNew(window, array[i]);
            if(median_window_full(window)) {
                medianwindow_result(window, result);
                result++;
            }
        }
    }

    free(startMemPtr);
    startMemPtr = NULL;
    memory = NULL;
    return true;
}

bool sliding_tiny_medianwindow(double *restrict array, size_t length, size_t windowSize,
    size_t steps, double *restrict result) {
    if(!valid_window(array, length, windowSize, steps, result))
        return false;

    char *memory = malloc(SIZE_OF_TINY_MEDIAN_WINDOW);
    if(memory == NULL)
        return false;

    char *startPtr = memory;
    Tiny_MedianWindow *window;
    tiny_medianwindow_initialize(&memory, windowSize, steps, &window);

    for(size_t i = 0; i < length; i++) {
        if(tiny_medianwindow_full(window))
            tiny_medianwindow_move_tail(window);

        tiny_medianwindow_move_head(window);
        if((tiny_medianwindow_full(window)) && (tiny_medianwindow_steps_reached(window))) {
            tiny_medianwindow_result(window, array, result);
            result++;
        }
    }

    free(startPtr);
    startPtr = NULL;
    memory = NULL;
    return true;
}

static inline bool median_window_full(MedianWindow *window) {
    return (window->currentSize == window->windowSize);
}

static inline bool median_window_steps_reached(MedianWindow *window) {
    if(window->stepDistance == 0) {
        window->stepDistance = window->steps - 1;
        return true;
    }

    window->stepDistance -= 1;
    return false;
}

static inline bool tiny_medianwindow_full(Tiny_MedianWindow *window) {
    const size_t currentSize = (window->headPtr - window->tailPtr);
    return (currentSize == (size_t) window->windowSize);
}

static inline bool tiny_medianwindow_steps_reached(Tiny_MedianWindow *window) {
    if(window->stepDistance == 0) {
        window->stepDistance = window->steps - 1;
        return true;
    }

    window->stepDistance -= 1;
    return false;
}
