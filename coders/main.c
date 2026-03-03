#include "args.h"
#include "coder.h"
#include "monitor.h"
#include "sim_stop.h"
#include "time.h"

#include <string.h>

typedef int (*t_init_fn)(t_sim *);
typedef void (*t_cleanup_fn)(t_sim *);

static int cleanup_all(t_sim *sim, int count, int error_code)
{
    t_cleanup_fn cleanup_stack[5];

    cleanup_stack[0] = sim_destroy_sync;
    cleanup_stack[1] = sim_destroy_dongles;
    cleanup_stack[2] = sim_destroy_coders;
    cleanup_stack[3] = monitor_join_thread;
    cleanup_stack[4] = coder_join_threads;
    if (error_code)
        sim_set_stop(sim);
    while (--count >= 0)
        cleanup_stack[count](sim);
    return (error_code);
}

int main(int ac, char **av)
{
    t_sim sim;
    t_init_fn init_stack[4];
    int i;

    memset(&sim, 0, sizeof(sim));
    if (!parse_args(&sim, ac, av))
        return (print_usage(av[0]), 1);
    if (!sim_init_sync(&sim))
        return (1);
    sim_mark_start(&sim);

    init_stack[0] = sim_init_dongles;
    init_stack[1] = sim_init_coders;
    init_stack[2] = monitor_start_thread;
    init_stack[3] = coder_start_threads;

    i = 0;
    while (++i <= 4)
        if (!init_stack[i - 1](&sim))
            return (cleanup_all(&sim, i, 1));

    while (!sim_should_stop(&sim))
        sleep_ms_precise(&sim, 1);

    return (cleanup_all(&sim, i, 0));
}