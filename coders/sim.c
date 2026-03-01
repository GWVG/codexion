#include "sim.h"

#include "dongle.h"

#include <stdlib.h>

int sim_init_dongles(t_sim *sim, int heap_capacity)
{
    int i;

    if (!sim || sim->coder_count <= 0 || heap_capacity <= 0)
        return (0);
    sim->dongles = (t_dongle *)malloc(sizeof(t_dongle) * sim->coder_count);
    if (!sim->dongles)
        return (0);
    i = 0;
    while (i < sim->coder_count)
    {
        if (!dongle_init(&sim->dongles[i], heap_capacity))
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
