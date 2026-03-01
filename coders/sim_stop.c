#include "sim_stop.h"

void sim_set_stop(t_sim *sim)
{
    if (!sim)
        return;
    pthread_mutex_lock(&sim->stop_mutex);
    sim->stop = 1;
    pthread_mutex_unlock(&sim->stop_mutex);
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