
#include <stdio.h>
#include <time.h>
#include "asoundlib.h"

void frite_open(snd_rawmidi_t **hin)
{
    char *devname = "hw:2,0,0"; //This appears to not change
    int result = 0;

    result = snd_rawmidi_open(hin, NULL, devname, SND_RAWMIDI_TYPE_HW);
    printf("Open Result: %i\n", result);
}

void print_time()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    printf("Time: %ld.%ld ", ts.tv_sec, ts.tv_nsec);
}

struct timespec get_time(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts;
}

struct timespec diff_time(struct timespec start, struct timespec end)
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

        if((delta.tv_sec == period.tv_sec &&
            delta.tv_nsec >= period.tv_nsec) ||
           (delta.tv_sec > period.tv_sec))
            return;
    }
}


int main(void)
{
    snd_rawmidi_t *handle_in = NULL;
    char buf[256];
    ssize_t size;
    int i;

    frite_open(&handle_in);

    while(1)
    {
        size = snd_rawmidi_read(handle_in,(void*)buf,256);
        print_time();
        wait_time(1000);
        printf("size: %i ", size);

        for(i=0;i<size;i++)
            printf("%hhx", buf[i]);


        printf("\n");
    }

    snd_rawmidi_drain(handle_in);
    snd_rawmidi_close(handle_in);
    return 0;
}
