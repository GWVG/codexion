#include "sim.h"

#include "coder.h"
#include "dongle.h"

#include <stdlib.h>
#include <sys/time.h>

void sim_init(t_sim *sim, int *v)
{
    if (!sim || !v)
        return;
    sim->scheduler = v[0];
    sim->coder_count = v[1];
    sim->time_to_burnout_ms = (long)v[2];
    sim->time_to_compile_ms = (long)v[3];
    sim->time_to_debug_ms = (long)v[4];
    sim->time_to_refactor_ms = (long)v[5];
    sim->required_compiles = v[6];
    sim->dongle_cooldown_ms = (long)v[7];
    sim->start_ms = 0;
    sim->stop = 0;
    sim->coders = NULL;
    sim->dongles = NULL;
}

int sim_init_sync(t_sim *sim)
{
    if (!sim)
        return (0);
    sim->stop = 0;
    if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
        return (0);
    if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
    {
        pthread_mutex_destroy(&sim->stop_mutex);
        return (0);
    }
    return (1);
}

void sim_destroy_sync(t_sim *sim)
{
    if (!sim)
        return;
    pthread_mutex_destroy(&sim->print_mutex);
    pthread_mutex_destroy(&sim->stop_mutex);
}

void sim_mark_start(t_sim *sim)
{
    struct timeval tv;

    if (!sim)
        return;
    gettimeofday(&tv, NULL);
    sim->start_ms = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000L);
}

int sim_init_dongles(t_sim *sim)
{
    int i;

    if (!sim || sim->coder_count <= 0)
        return (0);
    sim->dongles = malloc(sizeof(t_dongle) * sim->coder_count);
    if (!sim->dongles)
        return (0);
    i = 0;
    while (i < sim->coder_count)
    {
        if (!dongle_init(&sim->dongles[i], sim->coder_count))
        {
            while (--i >= 0)
                dongle_destroy(&sim->dongles[i]);
            free(sim->dongles);
            sim->dongles = NULL;
            return (0);
        }
        i++;
    }
    return (1);
}

void sim_destroy_dongles(t_sim *sim)
{
    int i;

    if (!sim || !sim->dongles || sim->coder_count <= 0)
        return;
    i = 0;
    while (i < sim->coder_count)
    {
        dongle_destroy(&sim->dongles[i]);
        i++;
    }
    free(sim->dongles);
    sim->dongles = NULL;
}

int sim_init_coders(t_sim *sim)
{
    int i;

    if (!sim || !sim->dongles || sim->coder_count <= 0)
        return (0);
    sim->coders = malloc(sizeof(t_coder) * sim->coder_count);
    if (!sim->coders)
        return (0);
    i = 0;
    while (i < sim->coder_count)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].last_compile_start_ms = sim->start_ms;
        sim->coders[i].compile_count = 0;
        sim->coders[i].compiling = 0;
        sim->coders[i].sim = sim;
        sim->coders[i].left_dongle = &sim->dongles[i];
        sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->coder_count];
        if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
        {
            while (--i >= 0)
                pthread_mutex_destroy(&sim->coders[i].state_mutex);
            free(sim->coders);
            sim->coders = NULL;
            return (0);
        }
        i++;
    }
    return (1);
}

void sim_destroy_coders(t_sim *sim)
{
    int i;

    if (!sim || !sim->coders || sim->coder_count <= 0)
        return;
    i = 0;
    while (i < sim->coder_count)
    {
        pthread_mutex_destroy(&sim->coders[i].state_mutex);
        i++;
    }
    free(sim->coders);
    sim->coders = NULL;
}
