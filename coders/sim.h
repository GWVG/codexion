#ifndef SIM_H
#define SIM_H

#include <pthread.h>

#define CODX_SCHED_FIFO 0
#define CODX_SCHED_EDF 1

typedef struct s_coder t_coder;
typedef struct s_dongle t_dongle;

typedef struct s_sim
{
	int coder_count;
	long time_to_burnout_ms;
	long time_to_compile_ms;
	long time_to_debug_ms;
	long time_to_refactor_ms;
	long dongle_cooldown_ms;
	int scheduler;
	int required_compiles;
	long start_ms;
	int stop;
	pthread_mutex_t stop_mutex;
	pthread_mutex_t print_mutex;
	t_coder *coders;
	t_dongle *dongles;
} t_sim;

void sim_init(t_sim *sim, int *v);
int sim_init_sync(t_sim *sim);
void sim_destroy_sync(t_sim *sim);
void sim_mark_start(t_sim *sim);
int sim_init_dongles(t_sim *sim);
void sim_destroy_dongles(t_sim *sim);

#endif
