/**
 * @file median_window.c
 * @author Jens Trappmann (jens.trappmann@icloud.com)
 * @brief This file implements a double-heap sliding median window.
 * @note The implementation follows the same general concept as other implementations,
 *       such as Bottleneck (https://github.com/pydata/bottleneck).
 * @version 0.1
 * @date 2025-12-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "median_window.h"

static inline size_t maxheap_put(MedianWindow *restrict window, HeapNode *restrict targetNode);
static void maxheap_heapifyUp(HeapNode *restrict *restrict maxHeapNodes, size_t position);
static void maxheap_heapifyDown(HeapNode *restrict *restrict maxHeapNodes, size_t heapLength, size_t position);
static void maxheap_largestChild(HeapNode *restrict *restrict maxHeapNodes, size_t heapLength, size_t position,
    size_t *restrict childPosition);
static inline size_t minheap_put(MedianWindow *restrict window, HeapNode *restrict targetNode);
static void minheap_heapifyUp(HeapNode *restrict *restrict minHeapNodes, size_t position);
static void minheap_heapifyDown(HeapNode *restrict *restrict minHeapNodes, size_t heapLength, size_t position);
static void minheap_smallestChild(HeapNode *restrict *restrict minHeapNodes, size_t heapLength, size_t position,
    size_t *restrict childPosition);
static void heaps_rebalance(MedianWindow *restrict window);
static inline size_t heap_calculate_children(size_t heapLength, size_t position);

void medianwindow_initialize(char **memory, size_t windowSize, size_t steps, MedianWindow **window) {
    MedianWindow *resultWindow = (MedianWindow* ) __builtin_assume_aligned(*memory, STD_ALIGNMENT);
    *memory += SIZE_OF_MEDIANWINDOW;

    const bool evenWindow = ((windowSize % 2) == 0);
    const size_t heapLength = (windowSize / 2);
    const size_t maxHeapLength = (evenWindow) ? heapLength : (heapLength + 1);
    const size_t maxHeapMem = (maxHeapLength * SIZE_OF_HEAPNODE_PTR);
    HeapNode **maxHeapStartingNode = (HeapNode** ) __builtin_assume_aligned(*memory, STD_ALIGNMENT);
    *memory += maxHeapMem;

    const size_t minHeapLength = (heapLength);
    const size_t minHeapMem = (minHeapLength * SIZE_OF_HEAPNODE_PTR);
    HeapNode **minHeapStartingNode = (HeapNode** ) __builtin_assume_aligned(*memory, STD_ALIGNMENT);
    *memory += minHeapMem;

    const size_t neededNodeMem = (windowSize * SIZE_OF_HEAPNODE);
    HeapNode *nodeDataStartingNode = (HeapNode* ) __builtin_assume_aligned(*memory, STD_ALIGNMENT);
    *memory += neededNodeMem;

    resultWindow->windowSize = windowSize;
    resultWindow->currentSize = 0;
    resultWindow->steps = steps;
    resultWindow->stepDistance = 0;
    resultWindow->maxHeap = maxHeapStartingNode;
    resultWindow->maxHeapLength = 0;
    resultWindow->minHeap = minHeapStartingNode;
    resultWindow->minHeapLength = 0;
    resultWindow->head = NULL;
    resultWindow->tail = NULL;
    resultWindow->nodes = nodeDataStartingNode;
    *window = resultWindow;
}

void medianwindow_addNew(MedianWindow *restrict window, double value) {
    if(!isfinite(value))
        return;

    HeapNode *inputNode = &(window->nodes[window->currentSize]);
    inputNode->value = value;

    if(__builtin_expect((window->maxHeapLength == 0), 0)) {
        maxheap_put(window, inputNode);
        window->tail = inputNode;
    } else {
        if(window->maxHeapLength > window->minHeapLength) {
            const size_t inputPosition = minheap_put(window, inputNode);
            minheap_heapifyUp(window->minHeap, inputPosition);
        } else {
            const size_t inputPosition = maxheap_put(window, inputNode);
            maxheap_heapifyUp(window->maxHeap, inputPosition);
        }

        heaps_rebalance(window);
        window->head->next = inputNode;
    }

    window->head = inputNode;
    window->currentSize += 1;
}

void medianwindow_updateOld(MedianWindow *restrict window, double value) {
    if(!isfinite(value))
        return;

    HeapNode *tailNode = window->tail;
    const double oldValue = tailNode->value;
    tailNode->value = value;
    window->tail = tailNode->next;
    window->head->next = tailNode;
    window->head = tailNode;

    if(tailNode->type == MAX_HEAP) {
        if(value > oldValue) {
            maxheap_heapifyUp(window->maxHeap, tailNode->position);
            heaps_rebalance(window);
        } else {
            maxheap_heapifyDown(window->maxHeap, window->maxHeapLength, tailNode->position);
        }
    } else {
        if(value < oldValue) {
            minheap_heapifyUp(window->minHeap, tailNode->position);
            heaps_rebalance(window);
        } else {
            minheap_heapifyDown(window->minHeap, window->minHeapLength, tailNode->position);
        }
    }
}

void medianwindow_result(MedianWindow *restrict window, double *restrict resultDest) {
    if(window->maxHeapLength != window->minHeapLength) {
        *resultDest = window->maxHeap[0]->value;
        return;
    }

    *resultDest = (window->maxHeap[0]->value + window->minHeap[0]->value) / 2;
}

size_t medianwindow_est_mem(size_t windowSize) {
    const size_t neededNodesPtrMem = (windowSize * SIZE_OF_HEAPNODE_PTR);
    const size_t neededNodesMem = (windowSize * SIZE_OF_HEAPNODE);
    return (SIZE_OF_MEDIANWINDOW + neededNodesPtrMem + neededNodesMem);
}

static inline size_t maxheap_put(MedianWindow *restrict window, HeapNode *restrict targetNode) {
    const size_t inputPosition = window->maxHeapLength;
    targetNode->position = inputPosition;
    targetNode->type = MAX_HEAP;
    window->maxHeap[inputPosition] = targetNode;
    window->maxHeapLength += 1;
    return inputPosition;
}

static void maxheap_heapifyUp(HeapNode *restrict *restrict maxHeapNodes, size_t position) {
    HeapNode *targetNode = maxHeapNodes[position];
    while (position > 0) {
        const size_t parentPosition = HEAP_PARENT_FORMULAR(position);
        HeapNode *parentNode = maxHeapNodes[parentPosition];
        if(targetNode->value <= parentNode->value)
            break;

        parentNode->position = position;
        maxHeapNodes[position] = parentNode;
        position = parentPosition;
    }

    targetNode->position = position;
    maxHeapNodes[position] = targetNode;
}

static void maxheap_heapifyDown(HeapNode *restrict *restrict maxHeapNodes, size_t heapLength, size_t position) {
    size_t target;
    maxheap_largestChild(maxHeapNodes, heapLength, position, &target);

    while (target != position) {
        HeapNode *positionNode = maxHeapNodes[position];
        HeapNode *childNode = maxHeapNodes[target];
        positionNode->position = target;
        maxHeapNodes[target] = positionNode;
        childNode->position = position;
        maxHeapNodes[position] = childNode;
        position = target;
        maxheap_largestChild(maxHeapNodes, heapLength, position, &target);
    }
}

static void maxheap_largestChild(HeapNode *restrict *restrict maxHeapNodes, size_t heapLength, size_t position,
    size_t *restrict childPosition) {
    const size_t minChildPosition = HEAP_CHILDREN_FORMULAR(position, 1);
    const size_t numChildren = heap_calculate_children(heapLength, position);

    switch (numChildren) {
        case 8: if(maxHeapNodes[minChildPosition + 7]->value > maxHeapNodes[position]->value)
            position = (minChildPosition + 7);
        case 7: if(maxHeapNodes[minChildPosition + 6]->value > maxHeapNodes[position]->value)
            position = (minChildPosition + 6);
        case 6: if(maxHeapNodes[minChildPosition + 5]->value > maxHeapNodes[position]->value)
            position = (minChildPosition + 5);
        case 5: if(maxHeapNodes[minChildPosition + 4]->value > maxHeapNodes[position]->value)
            position = (minChildPosition + 4);
        case 4: if(maxHeapNodes[minChildPosition + 3]->value > maxHeapNodes[position]->value)
            position = (minChildPosition + 3);
        case 3: if(maxHeapNodes[minChildPosition + 2]->value > maxHeapNodes[position]->value)
            position = (minChildPosition + 2);
        case 2: if(maxHeapNodes[minChildPosition + 1]->value > maxHeapNodes[position]->value)
            position = (minChildPosition + 1);
        case 1: if(maxHeapNodes[minChildPosition]->value > maxHeapNodes[position]->value)
            position = (minChildPosition);
        case 0: break;
    }

    *childPosition = position;
}

static inline size_t minheap_put(MedianWindow *restrict window, HeapNode *restrict targetNode) {
    const size_t inputPosition = window->minHeapLength;
    targetNode->position = inputPosition;
    targetNode->type = MIN_HEAP;
    window->minHeap[inputPosition] = targetNode;
    window->minHeapLength += 1;
    return inputPosition;
}

static void minheap_heapifyUp(HeapNode *restrict *restrict minHeapNodes, size_t position) {
    HeapNode *targetNode = minHeapNodes[position];
    while (position > 0) {
        const size_t parentPosition = HEAP_PARENT_FORMULAR(position);
        HeapNode *parentNode = minHeapNodes[parentPosition];
        if(targetNode->value >= parentNode->value)
            break;

        parentNode->position = position;
        minHeapNodes[position] = parentNode;
        position = parentPosition;
    }

    targetNode->position = position;
    minHeapNodes[position] = targetNode;
}

static void minheap_heapifyDown(HeapNode *restrict *restrict minHeapNodes, size_t heapLength, size_t position) {
    size_t target;
    minheap_smallestChild(minHeapNodes, heapLength, position, &target);

    while (target != position) {
        HeapNode *positionNode = minHeapNodes[position];
        HeapNode *childNode = minHeapNodes[target];
        positionNode->position = target;
        minHeapNodes[target] = positionNode;
        childNode->position = position;
        minHeapNodes[position] = childNode;
        position = target;
        minheap_smallestChild(minHeapNodes, heapLength, position, &target);
    }
}

static void minheap_smallestChild(HeapNode *restrict *restrict minHeapNodes, size_t heapLength, size_t position,
    size_t *restrict childPosition) {
    const size_t minChildPosition = HEAP_CHILDREN_FORMULAR(position, 1);
    const size_t numChildren = heap_calculate_children(heapLength, position);

    switch (numChildren) {
        case 8: if(minHeapNodes[minChildPosition + 7]->value < minHeapNodes[position]->value)
            position = (minChildPosition + 7);
        case 7: if(minHeapNodes[minChildPosition + 6]->value < minHeapNodes[position]->value)
            position = (minChildPosition + 6);
        case 6: if(minHeapNodes[minChildPosition + 5]->value < minHeapNodes[position]->value)
            position = (minChildPosition + 5);
        case 5: if(minHeapNodes[minChildPosition + 4]->value < minHeapNodes[position]->value)
            position = (minChildPosition + 4);
        case 4: if(minHeapNodes[minChildPosition + 3]->value < minHeapNodes[position]->value)
            position = (minChildPosition + 3);
        case 3: if(minHeapNodes[minChildPosition + 2]->value < minHeapNodes[position]->value)
            position = (minChildPosition + 2);
        case 2: if(minHeapNodes[minChildPosition + 1]->value < minHeapNodes[position]->value)
            position = (minChildPosition + 1);
        case 1: if(minHeapNodes[minChildPosition]->value < minHeapNodes[position]->value)
            position = (minChildPosition);
        case 0: break;
    }

    *childPosition = position;
}

static void heaps_rebalance(MedianWindow *restrict window) {
    HeapNode *restrict maxHeapRoot = window->maxHeap[0];
    HeapNode *restrict minHeapRoot = window->minHeap[0];
    if(maxHeapRoot->value < minHeapRoot->value) {
        return;
    }

    window->maxHeap[0] = minHeapRoot;
    minHeapRoot->type = MAX_HEAP;
    window->minHeap[0] = maxHeapRoot;
    maxHeapRoot->type = MIN_HEAP;
    maxheap_heapifyDown(window->maxHeap, window->maxHeapLength, 0);
    minheap_heapifyDown(window->minHeap, window->minHeapLength, 0);
}

static inline size_t heap_calculate_children(size_t heapLength, size_t position) {
    const size_t minChildPosition = HEAP_CHILDREN_FORMULAR(position, 1);
    const size_t maxChildPosition = HEAP_CHILDREN_FORMULAR(position, K_ARY_HEAP_CHILDREN);
    return (minChildPosition >= heapLength) ? 0 : (maxChildPosition >= heapLength) ?
        (heapLength - minChildPosition) : ((maxChildPosition - minChildPosition) + 1);
}
