#ifndef JTIME_H_
#define JTIME_H_

#include <time.h>


void print_time(void);
struct timespec get_time(void);
int time_elapsed(struct timespec delta, struct timespec period);
void wait_time(int msecs);

typedef struct
{
  struct timespec ev;
  struct timespec period;
} jtime_t;

void timer_init(jtime_t *t, int period_msec);
int timer_check(jtime_t *t);

#endif  /* JTIME_H_ */
