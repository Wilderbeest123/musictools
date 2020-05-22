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
            square_wave(&k->notes[i], buffer, size);
            //tri_wave(&k->notes[i], buffer, size);
        }
    }
}


static uint8_t keys_get_note(uint8_t midi_val)
{
    return midi_val%12;
}

static uint16_t keys_get_notes(uint8_t* data, int num)
{
    uint16_t notes;
    int i;

    notes = 0;

    for(i=0; i<num; i++)
        notes |= 1 << data[i];

    printf("notes: %u\n", notes);
    return notes;
}


static int keys_num_thirds(int root, uint16_t notes)
{
    int num_min, num_maj;
    int min, maj;
    uint16_t new_bm;

    num_min = 0;
    num_maj = 0;
    min = (root+3)%12;
    maj = (root+4)%12;
    new_bm = notes & ~(1<<root)%12;

    /*
    printf("root: %d minmod: %u majmod: %u old bm: %u, new bm: %u testval: %u\n",
           root, (root+3)%12, (root+4)%11, notes, new_bm, ~1<<(root%12));
    */
    /*
    printf("------------------------------\n");
    printf("root: %d\n", root);
    printf("bm: %u\n", notes);
    printf("min: %d\n", min);
    printf("maj: %d\n", maj);
    */

    if(1<<min & notes) {
        //printf("MIN root: %d\n", min);
        num_min++;
        num_min += keys_num_thirds(min,new_bm);
    }

    if(1<<maj & notes) {
        //printf("MAJ root: %d\n", maj);
        num_maj++;
        num_maj += keys_num_thirds(maj,new_bm);
    }

    if(num_maj > num_min) {
        //printf("MAJ root: %d num_thirds: %d\n", root, num_maj);
        return num_maj;
    }

    //printf("MIN root: %d num_thirds: %d\n", root, num_min);
    return num_min;
}

static int keys_root_note(uint16_t notes)
{
    int idx, val, c_root, r_val = 0;
    c_root = -1;

    for(idx=0; idx<12; idx++)
    {
        if(1<<idx & notes) {
            val = keys_num_thirds(idx, notes);

            if(val > r_val) {
                c_root = idx;
                r_val = val;
            }
        }
    }

    return c_root;
}

static gl_char_t keys_get_note_char(int note, gl_charset_t *cset)
{
    gl_char_t c;

    switch(note)
    {
    case 0: c = charset_get_char(cset, 'C'); break;
    case 2: c = charset_get_char(cset, 'D'); break;
    case 4: c = charset_get_char(cset, 'E'); break;
    case 5: c = charset_get_char(cset, 'F'); break;
    case 7: c = charset_get_char(cset, 'G'); break;
    case 9: c = charset_get_char(cset, 'A'); break;
    case 11: c = charset_get_char(cset, 'B'); break;
    }

    return c;
}

void keys_draw_notes(keyboard_t *k, v2 pos, gl_charset_t *cset)
{
    int i,j, count, root;
    bool newline;
    uint16_t notes_bm;
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
    notes_bm = keys_get_notes(notes, count);
    root = keys_root_note(notes_bm);

    if(root != -1)
    {
        c = keys_get_note_char(root, cset);
        glBindTexture(GL_TEXTURE_2D, c.tid);

        color.r = 255;
        square_draw(pos.x, pos.y, c.size.x, c.size.y, color);
        pos.x += c.advance;
        color.r = 0;
    }

    for(i=0; i<count; i++)
    {
        newline = true;
        //printf("%hhu ", notes[i]);

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

    //if(newline)
    //    printf("\n");
}
