#ifndef CODER_H
#define CODER_H

#include <pthread.h>

typedef struct s_sim t_sim;
typedef struct s_dongle t_dongle;

typedef struct s_coder
{
    int id;
    pthread_t thread;
    long last_compile_start_ms;
    pthread_mutex_t state_mutex;
    int compile_count;
    int compiling;
    t_sim *sim;
    t_dongle *left_dongle;
    t_dongle *right_dongle;
} t_coder;

void *coder_run(void *arg);
int coder_start_threads(t_sim *sim);
void coder_join_threads(t_sim *sim);

#endif
