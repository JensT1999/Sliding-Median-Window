#ifndef MEDIAN_WINDOW_H
#define MEDIAN_WINDOW_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

#define K_ARY_HEAP_CHILDREN 8
#define HEAP_PARENT_FORMULAR(position) (position - 1) / K_ARY_HEAP_CHILDREN
#define HEAP_CHILDREN_FORMULAR(position, num_child) (position * K_ARY_HEAP_CHILDREN) + num_child

typedef enum HeapType {
    MAX_HEAP,
    MIN_HEAP
} HeapType;

typedef struct HeapNode {
    double value;
    size_t position;
    struct HeapNode *next;
    HeapType type;
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
    HeapNode *nodes;
    HeapNode *tail;
    HeapNode *head;
} MedianWindow;

void medianwindow_initialize(char **memory, size_t windowSize, size_t steps, MedianWindow **window);
void medianwindow_addNew(MedianWindow *restrict window, double value);
void medianwindow_updateOld(MedianWindow *restrict window, double value);
void medianwindow_result(MedianWindow *restrict window, double *restrict resultDest);
size_t medianwindow_est_mem(size_t windowSize);

#define SIZE_OF_HEAPNODE sizeof(HeapNode)
#define SIZE_OF_HEAPNODE_PTR sizeof(HeapNode*)
#define SIZE_OF_MEDIANWINDOW sizeof(MedianWindow)

#endif
