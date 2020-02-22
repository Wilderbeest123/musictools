
#include <stdint.h>
#include <stdio.h>
#include "jtime.h"
#include "asoundlib.h"

void frite_open(snd_rawmidi_t **hin)
{
    char *devname = "hw:2,0,0"; //This appears to not change
    int result = 0;

    result = snd_rawmidi_open(hin, NULL, devname, SND_RAWMIDI_TYPE_HW);
    printf("Open Result: %i\n", result);

    snd_rawmidi_nonblock(*hin, 1); //1 == NONBLOCK MODE
}

void frite_read(snd_rawmidi_t *in)
{
    ssize_t size;
    uint8_t buf[256];
    int i;

    size = snd_rawmidi_read(in,(void*)buf,256);

    if(size < 1)
        return;

    for(i=0;i<size;)
    {
        switch(buf[i])
        {
        case 0xfe: //
            i++;
            break;

        case 0x90:
            printf("ON: %hhx %hhx\n", buf[i+1], buf[i+2]);
            i+=3;
            break;

        case 0x80:
            printf("OFF: %hhx %hhx\n", buf[i+1], buf[i+2]);
            i+=3;
            break;

        case 0xb0:
            printf("PEDAL: %hhx %hhx\n", buf[i+1], buf[i+2]);
            i+=3;
            break;

        default:
            printf("%hhx\n", buf[i]);
            i++;
            break;
        }

    }
}

int main(void)
{
    snd_rawmidi_t *handle_in = NULL;
    jtime_t timer;

    frite_open(&handle_in);
    timer_init(&timer, 100);

    while(1)
    {
        if(timer_check(&timer)){
            //print_time();
            frite_read(handle_in);
        }
    }

    snd_rawmidi_drain(handle_in);
    snd_rawmidi_close(handle_in);
    return 0;
}
