#include "args.h"

#include <string.h>

int main(int ac, char **av)
{
    t_sim sim;

    memset(&sim, 0, sizeof(sim));
    if (!parse_args(&sim, ac, av))
        return (print_usage(av[0]), 1);
    if (!sim_init_sync(&sim))
        return (1);
    sim_mark_start(&sim);
    if (!sim_init_dongles(&sim))
        return (sim_destroy_sync(&sim), 1);
    sim_destroy_dongles(&sim);
    sim_destroy_sync(&sim);
    return (0);
}
