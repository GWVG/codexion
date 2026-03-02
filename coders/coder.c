#include "coder.h"

#include "dongle.h"
#include "log.h"
#include "sim_stop.h"
#include "time.h"

static unsigned long coder_next_arrival_seq(t_dongle *dongle)
{
    unsigned long seq;

    pthread_mutex_lock(&dongle->mtx);
    seq = ++dongle->arrival_seq;
    pthread_mutex_unlock(&dongle->mtx);
    return (seq);
}

static long coder_deadline_ms(t_coder *coder)
{
    long last_compile_start;

    pthread_mutex_lock(&coder->state_mutex);
    last_compile_start = coder->last_compile_start_ms;
    pthread_mutex_unlock(&coder->state_mutex);
    return (last_compile_start + coder->sim->time_to_burnout_ms);
}

static t_waiter coder_make_waiter(t_coder *coder, t_dongle *dongle)
{
    t_waiter waiter;

    waiter.coder_id = coder->id;
    waiter.key_primary = coder->sim->scheduler * coder_deadline_ms(coder);
    waiter.key_secondary = coder_next_arrival_seq(dongle);
    return (waiter);
}

static int coder_take_two_dongles(t_coder *coder)
{
    t_dongle *first;
    t_dongle *second;

    first = coder->left_dongle;
    second = coder->right_dongle;
    if (first > second)
    {
        first = coder->right_dongle;
        second = coder->left_dongle;
    }
    if (!dongle_take(coder->sim, first, coder_make_waiter(coder, first)))
        return (0);
    log_state(coder->sim, coder->id, "has taken a dongle");
    if (!dongle_take(coder->sim, second, coder_make_waiter(coder, second)))
        return (dongle_release(coder->sim, first), 0);
    log_state(coder->sim, coder->id, "has taken a dongle");
    return (1);
}

static void coder_release_two_dongles(t_coder *coder)
{
    dongle_release(coder->sim, coder->left_dongle);
    if (coder->right_dongle != coder->left_dongle)
        dongle_release(coder->sim, coder->right_dongle);
}

static void coder_set_compile_state(t_coder *coder, int compiling)
{
    pthread_mutex_lock(&coder->state_mutex);
    coder->compiling = compiling;
    if (compiling)
        coder->last_compile_start_ms = now_ms();
    else
        coder->compile_count++;
    pthread_mutex_unlock(&coder->state_mutex);
}

void *coder_run(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    if (!coder || !coder->sim)
        return (NULL);
    if (coder->left_dongle == coder->right_dongle)
    {
        while (!sim_should_stop(coder->sim))
            sleep_ms_precise(coder->sim, 1);
        return (NULL);
    }
    while (!sim_should_stop(coder->sim))
    {
        if (!coder_take_two_dongles(coder))
            break;
        coder_set_compile_state(coder, 1);
        log_state(coder->sim, coder->id, "is compiling");
        sleep_ms_precise(coder->sim, coder->sim->time_to_compile_ms);
        coder_set_compile_state(coder, 0);
        coder_release_two_dongles(coder);
        if (sim_should_stop(coder->sim))
            break;
        log_state(coder->sim, coder->id, "is debugging");
        sleep_ms_precise(coder->sim, coder->sim->time_to_debug_ms);
        if (sim_should_stop(coder->sim))
            break;
        log_state(coder->sim, coder->id, "is refactoring");
        sleep_ms_precise(coder->sim, coder->sim->time_to_refactor_ms);
    }
    return (NULL);
}

int coder_start_threads(t_sim *sim)
{
    int i;

    if (!sim || !sim->coders || sim->coder_count <= 0)
        return (0);
    i = 0;
    while (i < sim->coder_count)
    {
        if (pthread_create(&sim->coders[i].thread, NULL, coder_run,
                           &sim->coders[i]) != 0)
        {
            sim_set_stop(sim);
            while (--i >= 0)
                pthread_join(sim->coders[i].thread, NULL);
            return (0);
        }
        i++;
    }
    return (1);
}

void coder_join_threads(t_sim *sim)
{
    int i;

    if (!sim || !sim->coders || sim->coder_count <= 0)
        return;
    i = 0;
    while (i < sim->coder_count)
        pthread_join(sim->coders[i++].thread, NULL);
}
