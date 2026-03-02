#include "dongle.h"

#include "sim_stop.h"
#include "time.h"

static t_heap_node waiter_to_node(t_waiter w)
{
    t_heap_node n;

    n.coder_id = w.coder_id;
    n.key_primary = w.key_primary;
    n.key_secondary = w.key_secondary;
    return (n);
}

static int node_is_me(t_heap_node n, t_waiter me)
{
    return (n.coder_id == me.coder_id && n.key_primary == me.key_primary && n.key_secondary == me.key_secondary);
}

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

static void sift_up_local(t_heap *heap, int i)
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

static void sift_down_local(t_heap *heap, int i)
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

static void heap_remove_waiter(t_heap *heap, t_waiter me)
{
    int i;
    t_heap_node last;

    if (!heap || !heap->data || heap->size <= 0)
        return;
    i = 0;
    while (i < heap->size)
        if (node_is_me(heap->data[i++], me))
            break;
    if (i >= heap->size)
        return;
    heap->size--;
    if (i == heap->size)
        return;
    last = heap->data[heap->size];
    heap->data[i] = last;
    if (i > 0 && node_before(heap->data[i], heap->data[(i - 1) / 2]))
        sift_up_local(heap, i);
    else
        sift_down_local(heap, i);
}

int dongle_init(t_dongle *d, int capacity)
{
    if (!d)
        return (0);
    if (pthread_mutex_init(&d->mtx, NULL))
        return (0);
    if (pthread_cond_init(&d->cv, NULL))
        return (pthread_mutex_destroy(&d->mtx), 0);
    if (!heap_init(&d->wait_q, capacity))
        return (pthread_cond_destroy(&d->cv), pthread_mutex_destroy(&d->mtx), 0);
    d->available = 1;
    d->cooldown_until_ms = 0;
    d->arrival_seq = 0;
    return (1);
}

int dongle_take(t_sim *sim, t_dongle *d, t_waiter me)
{
    t_heap_node top;
    int is_top;

    if (!sim || !d)
        return (0);
    pthread_mutex_lock(&d->mtx);
    // put coder on the wait_q
    if (!heap_push(&d->wait_q, waiter_to_node(me)))
        return (pthread_mutex_unlock(&d->mtx), 0);
    // check until coder is at the top of the wait_q
    while (!sim_should_stop(sim))
    {
        is_top = heap_peek(&d->wait_q, &top) && node_is_me(top, me);
        if (d->available && now_ms() >= d->cooldown_until_ms && is_top)
            break;
        pthread_cond_wait(&d->cv, &d->mtx);
    }
    // check for early stop
    if (sim_should_stop(sim))
        return (heap_remove_waiter(&d->wait_q, me), pthread_mutex_unlock(&d->mtx), 0);
    // remove coder from wait_q and take dongle
    heap_pop(&d->wait_q, &top);
    d->available = 0;
    return (pthread_mutex_unlock(&d->mtx), 1);
}

void dongle_release(t_sim *sim, t_dongle *d)
{
    if (!sim || !d)
        return;
    pthread_mutex_lock(&d->mtx);
    d->available = 1;
    d->cooldown_until_ms = now_ms() + sim->dongle_cooldown_ms;
    pthread_cond_broadcast(&d->cv);
    pthread_mutex_unlock(&d->mtx);
}

void dongle_destroy(t_dongle *d)
{
    if (!d)
        return;
    heap_destroy(&d->wait_q);
    pthread_cond_destroy(&d->cv);
    pthread_mutex_destroy(&d->mtx);
}
