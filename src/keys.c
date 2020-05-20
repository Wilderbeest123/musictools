#include "keys.h"

static void keys_midi_handle_on(void *this, uint8_t note, uint8_t velocity)
{
    keyboard_t *k = CONTAINER_OF(this, keyboard_t, ev);
    int i;

    k->keys[note/8] |= (1 << note%8);

    for(i=0;i<12;i++)
    {
        if(k->notes[i].freq == 0.0) {
            tone_init(&k->notes[i], midi_to_freq(note));
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
    {
        if(k->notes[i].freq != 0.0) {
            sine_wave(&k->notes[i], buffer, size);
        }
    }
}


static uint8_t keys_get_note(uint8_t midi_val)
{
    return midi_val%12;
}

void keys_draw_notes(keyboard_t *k, v2 pos, gl_charset_t *cset)
{
    int i,j, count;
    bool newline;
    uint8_t notes[20] = {0};
    gl_char_t c;
    gl_color_t color = COLOR_INIT(0, 0, 0, 255);

    //determine what notes are being pressed.
    count = 0;

    for(i=0;i<16;i++)
    {
        for(j=0;j<=8;j++)
        {
            if(k->keys[i] & (1 << j)) {
                //newline = true;
                //printf("%hhx %hhu ", (i*8)+(j), keys_get_note((i*8)+j));
                notes[count++] = keys_get_note((i*8+j));
            }
        }
    }

    //Based on the notes determine whether sharps or flats.

    for(i=0; i<count; i++)
    {
        newline = true;
        printf("%hhu ", notes[i]);

        switch(notes[i])
        {
        case 0: c = charset_get_char(cset, 'C'); break;
        case 2: c = charset_get_char(cset, 'D'); break;
        case 4: c = charset_get_char(cset, 'E'); break;
        case 5: c = charset_get_char(cset, 'F'); break;
        case 7: c = charset_get_char(cset, 'G'); break;
        case 9: c = charset_get_char(cset, 'A'); break;
        case 11: c = charset_get_char(cset, 'B'); break;
        }

        glBindTexture(GL_TEXTURE_2D, c.tid);
        square_draw(pos.x, pos.y, c.size.x, c.size.y, color);
        pos.x += c.advance;
    }

    if(newline)
        printf("\n");
}
