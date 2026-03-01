#include "log.h"

#include "sim_stop.h"
#include "time.h"

#include <stdio.h>
#include <string.h>

void log_state(t_sim *sim, int id, const char *msg)
{
    int should_print;

    if (!sim || !msg)
        return;
    pthread_mutex_lock(&sim->print_mutex);
    should_print = !sim_should_stop(sim) || strcmp(msg, "burned out") == 0;
    if (should_print)
        printf("%ld %d %s\n", since_start_ms(sim), id, msg);
    pthread_mutex_unlock(&sim->print_mutex);
}
