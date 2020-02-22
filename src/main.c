
#include <stdbool.h>
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

typedef struct
{
    uint8_t keys[16]; //bitmask representing whether key is pressed;
} keyboard_t;

void keys_init(keyboard_t *k)
{
    int i;

    for(i=0;i<16;i++)
        k->keys[i] = 0;
}

void keys_midi_handle_on(keyboard_t *k, uint8_t note, uint8_t velocity)
{
    k->keys[note/8] |= (1 << note%8);
}

void keys_midi_handle_off(keyboard_t *k, uint8_t note, uint8_t velocity)
{
    k->keys[note/8] &= ~(1 << note%8);
}

void keys_print_notes(keyboard_t *k)
{
    int i,j;
    bool newline;

    newline = false;

    for(i=0;i<16;i++)
    {
        for(j=0;j<=8;j++)
        {
            if(k->keys[i] & (1 << j)) {
                newline = true;
                printf("%hhx ", (i*8)+(j));
            }
        }
    }

    if(newline)
        printf("\n");
}

void frite_read(snd_rawmidi_t *in, keyboard_t *k)
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
            keys_midi_handle_on(k, buf[i+1], buf[i+2]);
            i+=3;
            break;

        case 0x80:
            printf("OFF: %hhx %hhx\n", buf[i+1], buf[i+2]);
            keys_midi_handle_off(k, buf[i+1], buf[i+2]);
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

    keys_print_notes(k);
}

int main(void)
{
    snd_rawmidi_t *handle_in = NULL;
    jtime_t timer;
    keyboard_t k;

    keys_init(&k);
    frite_open(&handle_in);
    timer_init(&timer, 20);

    while(1)
    {
        if(timer_check(&timer)){
            //print_time();
            frite_read(handle_in, &k);
        }
    }

    snd_rawmidi_drain(handle_in);
    snd_rawmidi_close(handle_in);
    return 0;
}
