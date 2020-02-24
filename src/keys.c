#include "keys.h"

static void keys_midi_handle_on(void *this, uint8_t note, uint8_t velocity)
{
    keyboard_t *k = CONTAINER_OF(this, keyboard_t, ev);
    int i;

    k->keys[note/8] |= (1 << note%8);

    for(i=0;i<12;i++)
    {
        if(k->notes[i].freq == 0.0) {
            sine_init(&k->notes[i], midi_to_freq(note));
            return;
        }
    }
}

static void keys_midi_handle_off(void *this, uint8_t note, uint8_t velocity)
{
    keyboard_t *k = CONTAINER_OF(this, keyboard_t, ev);
    int i;

    k->keys[note/8] &= ~(1 << note%8);

    for(i=0;i<12;i++)
    {
        if(k->notes[i].freq == (int)midi_to_freq(note))
        {
            k->notes[i].freq = 0.0;
            return;
        }
    }
}

void keys_init(keyboard_t *k)
{
    int i;

    for(i=0;i<16;i++)
        k->keys[i] = 0;

    for(i=0;i<12;i++)
        k->notes[i].freq = 0.0;

    k->ev.press_on = keys_midi_handle_on;
    k->ev.press_off = keys_midi_handle_off;
    k->ev.pedal = NULL;
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

void keys_populate_buffer(keyboard_t *k, uint16_t *buffer, uint32_t size)
{
    int i;

    for(i=0;i<12;i++)
        if(k->notes[i].freq != 0.0)
            sine_wave(&k->notes[i], buffer, size);
}
