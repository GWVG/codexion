#ifndef MONITOR_H
#define MONITOR_H

#include "sim.h"

void *monitor_run(void *arg);
int monitor_start_thread(t_sim *sim);
void monitor_join_thread(t_sim *sim);

#endif
