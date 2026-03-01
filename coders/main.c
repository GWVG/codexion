#include "heap.h"

#include <stdio.h>

static int is_non_decreasing(t_heap_node prev, t_heap_node cur)
{
    if (cur.key_primary > prev.key_primary)
        return (1);
    if (cur.key_primary < prev.key_primary)
        return (0);
    return (cur.key_secondary >= prev.key_secondary);
}

static int run_case(const char *label, t_heap_node *nodes, int count)
{
    t_heap heap;
    t_heap_node cur;
    t_heap_node prev;
    int ok;
    int i;

    if (!heap_init(&heap, count))
        return (0);
    i = 0;
    while (i < count)
    {
        if (!heap_push(&heap, nodes[i]))
        {
            heap_destroy(&heap);
            return (0);
        }
        i++;
    }
    printf("%s pop order:\n", label);
    ok = heap_pop(&heap, &prev);
    if (!ok)
    {
        heap_destroy(&heap);
        return (0);
    }
    printf("coder=%d primary=%ld secondary=%ld\n",
           prev.coder_id, prev.key_primary, prev.key_secondary);
    i = 1;
    while (i < count)
    {
        heap_pop(&heap, &cur);
        printf("coder=%d primary=%ld secondary=%ld\n",
               cur.coder_id, cur.key_primary, cur.key_secondary);
        if (!is_non_decreasing(prev, cur))
            ok = 0;
        prev = cur;
        i++;
    }
    heap_destroy(&heap);
    printf("%s verify: %s\n\n", label, ok ? "PASS" : "FAIL");
    return (ok);
}

int main(void)
{
    t_heap_node fifo_nodes[10];
    t_heap_node edf_nodes[10];
    int all_ok;

    fifo_nodes[0] = (t_heap_node){1, 50, 50};
    fifo_nodes[1] = (t_heap_node){2, 10, 10};
    fifo_nodes[2] = (t_heap_node){3, 70, 70};
    fifo_nodes[3] = (t_heap_node){4, 20, 20};
    fifo_nodes[4] = (t_heap_node){5, 90, 90};
    fifo_nodes[5] = (t_heap_node){6, 40, 40};
    fifo_nodes[6] = (t_heap_node){7, 30, 30};
    fifo_nodes[7] = (t_heap_node){8, 80, 80};
    fifo_nodes[8] = (t_heap_node){9, 60, 60};
    fifo_nodes[9] = (t_heap_node){10, 0, 0};
    edf_nodes[0] = (t_heap_node){1, 300, 5};
    edf_nodes[1] = (t_heap_node){2, 120, 2};
    edf_nodes[2] = (t_heap_node){3, 120, 1};
    edf_nodes[3] = (t_heap_node){4, 450, 9};
    edf_nodes[4] = (t_heap_node){5, 200, 4};
    edf_nodes[5] = (t_heap_node){6, 200, 3};
    edf_nodes[6] = (t_heap_node){7, 10, 7};
    edf_nodes[7] = (t_heap_node){8, 10, 6};
    edf_nodes[8] = (t_heap_node){9, 999, 10};
    edf_nodes[9] = (t_heap_node){10, 450, 8};
    all_ok = 1;
    if (!run_case("FIFO", fifo_nodes, 10))
        all_ok = 0;
    if (!run_case("EDF", edf_nodes, 10))
        all_ok = 0;
    return (all_ok ? 0 : 1);
}
