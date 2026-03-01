#ifndef TIME_H
# define TIME_H

# include "sim.h"

long	now_ms(void);
long	since_start_ms(t_sim *sim);
void	sleep_ms_precise(t_sim *sim, long ms);

#endif
