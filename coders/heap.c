#include "heap.h"

#include <stdlib.h>

static int node_before(t_heap_node a, t_heap_node b)
{
    if (a.key_primary < b.key_primary)
        return (1);
    if (a.key_primary > b.key_primary)
        return (0);
    return (a.key_secondary < b.key_secondary);
}

static void node_swap(t_heap_node *a, t_heap_node *b)
{
    t_heap_node tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

static void sift_up(t_heap *heap, int i)
{
    int parent;

    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (!node_before(heap->data[i], heap->data[parent]))
            break;
        node_swap(&heap->data[i], &heap->data[parent]);
        i = parent;
    }
}

static void sift_down(t_heap *heap, int i)
{
    int left;
    int right;
    int smallest;

    while (1)
    {
        left = (i * 2) + 1;
        right = left + 1;
        smallest = i;
        if (left < heap->size && node_before(heap->data[left], heap->data[smallest]))
            smallest = left;
        if (right < heap->size && node_before(heap->data[right], heap->data[smallest]))
            smallest = right;
        if (smallest == i)
            break;
        node_swap(&heap->data[i], &heap->data[smallest]);
        i = smallest;
    }
}

int heap_init(t_heap *heap, int capacity)
{
    if (!heap || capacity <= 0)
        return (0);
    heap->data = malloc(sizeof(t_heap_node) * capacity);
    if (!heap->data)
        return (0);
    heap->size = 0;
    heap->capacity = capacity;
    return (1);
}

int heap_push(t_heap *heap, t_heap_node node)
{
    if (!heap || !heap->data || heap->size >= heap->capacity)
        return (0);
    heap->data[heap->size] = node;
    sift_up(heap, heap->size);
    heap->size++;
    return (1);
}

int heap_peek(t_heap *heap, t_heap_node *out)
{
    if (!heap || !heap->data || heap->size <= 0 || !out)
        return (0);
    *out = heap->data[0];
    return (1);
}

int heap_pop(t_heap *heap, t_heap_node *out)
{
    if (!heap_peek(heap, out))
        return (0);
    heap->size--;
    heap->data[0] = heap->data[heap->size];
    sift_down(heap, 0);
    return (1);
}

void heap_destroy(t_heap *heap)
{
    if (!heap)
        return;
    if (heap->data)
        free(heap->data);
    heap->data = NULL;
    heap->size = 0;
    heap->capacity = 0;
}
