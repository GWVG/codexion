#include "monitor.h"

#include "coder.h"
#include "log.h"
#include "sim_stop.h"
#include "time.h"

static int monitor_find_burnout(t_sim *sim, int *burnout_id)
{
    int i;
    long now;
    long last_compile_start;

    if (!sim || !sim->coders || sim->coder_count <= 0 || !burnout_id)
        return (0);
    now = now_ms();
    i = 0;
    while (i < sim->coder_count)
    {
        pthread_mutex_lock(&sim->coders[i].state_mutex);
        last_compile_start = sim->coders[i].last_compile_start_ms;
        pthread_mutex_unlock(&sim->coders[i].state_mutex);
        if (now >= (last_compile_start + sim->time_to_burnout_ms))
        {
            *burnout_id = sim->coders[i].id;
            return (1);
        }
        i++;
    }
    return (0);
}

static int monitor_all_required_compiles_done(t_sim *sim)
{
    int i;
    int compile_count;

    if (!sim || !sim->coders || sim->coder_count <= 0)
        return (0);
    if (sim->required_compiles < 0)
        return (0);
    i = 0;
    while (i < sim->coder_count)
    {
        pthread_mutex_lock(&sim->coders[i].state_mutex);
        compile_count = sim->coders[i].compile_count;
        pthread_mutex_unlock(&sim->coders[i].state_mutex);
        if (compile_count < sim->required_compiles)
            return (0);
        i++;
    }
    return (1);
}

void *monitor_run(void *arg)
{
    t_sim *sim;
    int burnout_id;

    sim = (t_sim *)arg;
    if (!sim)
        return (NULL);
    burnout_id = 0;
    while (!sim_should_stop(sim))
    {
        if (monitor_find_burnout(sim, &burnout_id))
        {
            sim_set_stop(sim);
            log_state(sim, burnout_id, "burned out");
            break;
        }
        if (monitor_all_required_compiles_done(sim))
        {
            sim_set_stop(sim);
            break;
        }
        sleep_ms_precise(sim, 1);
    }
    return (NULL);
}

int monitor_start_thread(t_sim *sim)
{
    if (!sim || pthread_create(&sim->monitor_thread, NULL, monitor_run, sim))
        return (0);
    return (1);
}

void monitor_join_thread(t_sim *sim)
{
    if (!sim)
        return;
    pthread_join(sim->monitor_thread, NULL);
}
