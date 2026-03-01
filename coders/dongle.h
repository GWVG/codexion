#ifndef DONGLE_H
#define DONGLE_H

#include <pthread.h>
#include "heap.h"

typedef struct s_sim t_sim;

typedef struct s_waiter
{
    int coder_id;
    long key_primary;
    long key_secondary;
} t_waiter;

typedef struct s_dongle
{
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    int available;
    long cooldown_until_ms;
    t_heap wait_q;
    unsigned long arrival_seq;
} t_dongle;

int dongle_init(t_dongle *d, int capacity);
int dongle_take(t_sim *sim, t_dongle *d, t_waiter me);
void dongle_release(t_sim *sim, t_dongle *d);
void dongle_destroy(t_dongle *d);

#endif
