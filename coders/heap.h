#ifndef HEAP_H
#define HEAP_H

typedef struct s_heap_node
{
    int coder_id;
    long key_primary;
    long key_secondary;
} t_heap_node;

typedef struct s_heap
{
    t_heap_node *data;
    int size;
    int capacity;
} t_heap;

int heap_init(t_heap *heap, int capacity);
int heap_push(t_heap *heap, t_heap_node node);
int heap_peek(t_heap *heap, t_heap_node *out);
int heap_pop(t_heap *heap, t_heap_node *out);
void heap_destroy(t_heap *heap);

#endif
