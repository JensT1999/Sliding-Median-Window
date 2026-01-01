#ifndef MEDIAN_WINDOW_H
#define MEDIAN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define STD_ALIGNMENT 8
#define K_ARY_HEAP_CHILDREN 8
#define HEAP_PARENT_FORMULAR(position) (position - 1) / K_ARY_HEAP_CHILDREN
#define HEAP_CHILDREN_FORMULAR(position, num_child) (position * K_ARY_HEAP_CHILDREN) + num_child
#define SPC_NUMBER_INPUT_POSITION SIZE_MAX

typedef enum HeapType {
    MAX_HEAP,
    MIN_HEAP,
    SPC_NUMBER
} HeapType;

typedef struct HeapNode {
    double value; // 0 - 7
    size_t position; // 8 - 15
    struct HeapNode *next; // 16 - 23
    HeapType type; // 24 - 27
    bool isNaN;
} HeapNode;

typedef struct MedianWindow {
    size_t windowSize;
    size_t currentSize;
    size_t steps;
    size_t stepDistance;
    HeapNode **maxHeap;
    size_t maxHeapLength;
    HeapNode **minHeap;
    size_t minHeapLength;
    HeapNode *tail;
    HeapNode *head;
    HeapNode *nodes;
    size_t spcNumbers;
    bool ignoreNaNWindows;
} MedianWindow;

void medianwindow_initialize(char **memory, size_t windowSize, size_t steps, bool ignoreNaNWindows,
    MedianWindow **window);
void medianwindow_addNew(MedianWindow *restrict window, double value);
void medianwindow_updateOld(MedianWindow *restrict window, double value);
void medianwindow_result(MedianWindow *restrict window, double *restrict resultDest);
size_t medianwindow_est_mem(size_t windowSize);

#define SIZE_OF_HEAPNODE sizeof(HeapNode)
#define SIZE_OF_HEAPNODE_PTR sizeof(HeapNode*)
#define SIZE_OF_MEDIANWINDOW sizeof(MedianWindow)

#endif
