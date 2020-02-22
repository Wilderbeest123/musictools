#include "jtime.h"
#include <stdio.h>

void print_time()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    printf("Time: %ld.%ld\n", ts.tv_sec, ts.tv_nsec);
}

struct timespec get_time(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts;
}

static struct timespec diff_time(struct timespec start, struct timespec end)
{
    struct timespec ts;

    if(end.tv_nsec < start.tv_nsec)
    {
        ts.tv_sec = end.tv_sec-start.tv_sec-1;
        ts.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    }
    else
    {
        ts.tv_sec = end.tv_sec-start.tv_sec;
        ts.tv_nsec = end.tv_nsec-start.tv_nsec;
    }

    return ts;
}

int time_elapsed(struct timespec delta, struct timespec period)
{
    if((delta.tv_sec == period.tv_sec &&
        delta.tv_nsec >= period.tv_nsec) ||
       (delta.tv_sec > period.tv_sec))
        return 1;

    return 0;
}

void wait_time(int msecs)
{
    struct timespec start, period, delta;

    //Convert msecs to timespec
    period.tv_sec = msecs/1000;
    period.tv_nsec = (msecs%1000) * 1000000;

    start = get_time();

    while(1)
    {
        delta = diff_time(start, get_time());

        if(time_elapsed(delta, period))
            return;
    }
}

void timer_init(jtime_t *t, int period_msec)
{
    t->period.tv_sec = period_msec/1000;
    t->period.tv_nsec = (period_msec%1000) * 1000000;
    t->ev = get_time();
}

int timer_check(jtime_t *t)
{
    struct timespec ts, delta;

    ts = get_time();
    delta = diff_time(t->ev, ts);

    if(time_elapsed(delta, t->period))
    {
        t->ev = ts;
        return 1;
    }

    return 0;
}
