
#include <stdio.h>
#include "asoundlib.h"

void frite_open(snd_rawmidi_t **hin)
{
    char *devname = "hw:2,0,0"; //This appears to not change
    int result = 0;

    result = snd_rawmidi_open(hin, NULL, devname, SND_RAWMIDI_TYPE_HW);
    printf("Open Result: %i\n", result);
}

int main(void)
{
    snd_rawmidi_t *handle_in = NULL;
    char buf[256];
    ssize_t size;
    int i;


    frite_open(&handle_in);

    while(1) {
        size = snd_rawmidi_read(handle_in,(void*)buf,256);
        printf("size: %i, ", size);

        for(i=0;i<size;i++)
            printf("%hhx", buf[i]);

        printf("\n");
    }

    snd_rawmidi_drain(handle_in);
    snd_rawmidi_close(handle_in);
    return 0;
}
