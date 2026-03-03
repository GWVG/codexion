#include "sim_stop.h"

void sim_set_stop(t_sim *sim)
{
    int was_stopped;

    if (!sim)
        return;
    pthread_mutex_lock(&sim->stop_mutex);
    was_stopped = sim->stop;
    sim->stop = 1;
    pthread_mutex_unlock(&sim->stop_mutex);
    if (!was_stopped)
        sim_wake_all_dongles(sim);
}

int sim_should_stop(t_sim *sim)
{
    int stop;

    if (!sim)
        return (0);
    pthread_mutex_lock(&sim->stop_mutex);
    stop = sim->stop;
    pthread_mutex_unlock(&sim->stop_mutex);
    return (stop);
}