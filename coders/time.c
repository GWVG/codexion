#include "time.h"
#include "sim_stop.h"

#include <sys/time.h>
#include <unistd.h>

long now_ms(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
}

long since_start_ms(t_sim *sim)
{
	if (!sim)
		return (0);
	return (now_ms() - sim->start_ms);
}

void sleep_ms_precise(t_sim *sim, long ms)
{
	long start;
	long elapsed;

	if (ms <= 0)
		return;
	start = now_ms();
	while (!sim_should_stop(sim))
	{
		elapsed = now_ms() - start;
		if (elapsed >= ms)
			break;
		if ((ms - elapsed) > 1)
			usleep(500);
		else
			usleep(200);
	}
}
