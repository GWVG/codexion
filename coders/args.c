#include "args.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_nonneg_int(const char *s, int *out)
{
    long value;
    int i;

    if (!s || !s[0] || !out)
        return (0);
    value = 0;
    i = 0;
    while (s[i])
    {
        if (s[i] < '0' || s[i] > '9')
            return (0);
        value = (value * 10) + (s[i] - '0');
        if (value > INT_MAX)
            return (0);
        i++;
    }
    *out = atoi(s);
    return (1);
}

static int parse_scheduler(const char *s, int *out)
{
    if (!s || !out)
        return (0);
    if (!strcmp(s, "fifo"))
        *out = CODX_SCHED_FIFO;
    else if (!strcmp(s, "edf"))
        *out = CODX_SCHED_EDF;
    else
        return (0);
    return (1);
}

static int parse_numeric_args(char **av, int *v)
{
    int i;

    i = 0;
    while (++i <= 7)
        if (!parse_nonneg_int(av[i], &v[i]) || v[i] < (i <= 5))
            return (0);
    return (1);
}

void print_usage(const char *progname)
{
    fprintf(stderr,
            "Usage: %s number_of_coders time_to_burnout time_to_compile "
            "time_to_debug time_to_refactor number_of_compiles_required "
            "dongle_cooldown scheduler\n",
            progname);
    fprintf(stderr, "scheduler: fifo | edf\n");
}

int parse_args(t_sim *sim, int ac, char **av)
{
    int v[8];

    if (!sim || !av || ac != 9)
        return (0);
    if (!parse_numeric_args(av, v))
        return (0);
    if (!parse_scheduler(av[8], &v[0]))
        return (0);
    sim_init(sim, v);
    return (1);
}
