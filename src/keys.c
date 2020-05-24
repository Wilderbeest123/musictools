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

    k->rnote = render_notes_init(40);
}

static v2 keys_render_note(render_notes_t r, int note, keys_scale_t scale, v2 pos, gl_color_t col)
{
    gl_char_t c;
    bool is_black = false;

    switch(note)
    {
    case NOTE_C: c = charset_get_char(&r.cset, 'C'); goto render;
    case NOTE_D: c = charset_get_char(&r.cset, 'D'); goto render;
    case NOTE_E: c = charset_get_char(&r.cset, 'E'); goto render;
    case NOTE_F: c = charset_get_char(&r.cset, 'F'); goto render;
    case NOTE_G: c = charset_get_char(&r.cset, 'G'); goto render;
    case NOTE_A: c = charset_get_char(&r.cset, 'A'); goto render;
    case NOTE_B: c = charset_get_char(&r.cset, 'B'); goto render;
    }

    is_black = true;

    if(scale == SCALE_FLAT)
    {
        switch(note)
        {
        case NOTE_D_FLAT: c = charset_get_char(&r.cset, 'D'); goto render;
        case NOTE_E_FLAT: c = charset_get_char(&r.cset, 'E'); goto render;
        case NOTE_G_FLAT: c = charset_get_char(&r.cset, 'G'); goto render;
        case NOTE_A_FLAT: c = charset_get_char(&r.cset, 'A'); goto render;
        case NOTE_B_FLAT: c = charset_get_char(&r.cset, 'B'); goto render;
        }
    }

    switch(note)
    {
    case NOTE_C_SHARP: c = charset_get_char(&r.cset, 'C'); goto render;
    case NOTE_D_SHARP: c = charset_get_char(&r.cset, 'D'); goto render;
    case NOTE_F_SHARP: c = charset_get_char(&r.cset, 'F'); goto render;
    case NOTE_G_SHARP: c = charset_get_char(&r.cset, 'G'); goto render;
    case NOTE_A_SHARP: c = charset_get_char(&r.cset, 'A'); goto render;
    }

render:
    glBindTexture(GL_TEXTURE_2D, c.tid);
    square_draw(pos.x, pos.y, c.size.x, c.size.y, col);
    pos.x += c.advance;

    if(!is_black)
        return pos;

    if(scale == SCALE_FLAT) {
        glBindTexture(GL_TEXTURE_2D, r.f_tex);
        square_draw(pos.x-r.f_off.x, pos.y+r.f_off.y, r.fs_size, r.fs_size, col);
        pos.x += r.fs_size-r.f_off.x;
        return pos;
    }

    glBindTexture(GL_TEXTURE_2D, r.s_tex);
    square_draw(pos.x-r.s_off.x, pos.y+r.s_off.y, r.fs_size, r.fs_size, col);
    pos.x += r.fs_size-r.s_off.x;
    return pos;
}

render_notes_t render_notes_init(int fontsize)
{
    render_notes_t n;

    n.cset = gl_load_charset("res/OpenSans-Bold.ttf", fontsize, 'A', 'H');
    n.s_tex = gl_load_image_rgba("res/sharp.png");
    n.f_tex = gl_load_image_rgba("res/flat.png");

    n.fs_size = 2*n.cset.rsize/3;
    n.s_off.x = n.fs_size/8;
    n.s_off.y = 5*n.fs_size/8;

    n.f_off.x = 3*n.fs_size/8;
    n.f_off.y = 5*n.fs_size/8;

    printf("s_tex: %d f_tex: %d\n", n.s_tex, n.f_tex);
    printf("s_off.x: %d s_off.y: %d\n", n.s_off.x, n.s_off.y);
    printf("f_off.x: %d f_off.y: %d\n", n.f_off.x, n.f_off.y);

    return n;
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

    //printf("notes: %u\n", notes);
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

int keys_get_third(int root, uint16_t notes)
{
    int num_min, num_maj;
    int min, maj;
    uint16_t new_bm;

    num_min = 0;
    num_maj = 0;
    min = (root+3)%12;
    maj = (root+4)%12;
    new_bm = notes & ~(1<<root)%12;

    if(1<<min & notes) {
        num_min++;
        num_min += keys_num_thirds(min,new_bm);
    }

    if(1<<maj & notes) {
        num_maj++;
        num_maj += keys_num_thirds(maj,new_bm);
    }

    if(num_maj >= num_min)
        return maj;

    return min;
}

int keys_sharp_or_flat(int root, uint16_t notes)
{
    int idx, third;
    bool is_major;

    third = keys_get_third(root, notes);
    is_major = false;

    if((root+4)%12 == third)
        is_major = true;

    if(is_major) {
        //MAJOR SCALES
        switch(root)
        {
            //SHARP
        case NOTE_C:
        case NOTE_G:
        case NOTE_D:
        case NOTE_A:
        case NOTE_E:
        case NOTE_B:
            return SCALE_SHARP;

            //FLAT
        case NOTE_F:
        case NOTE_B_FLAT:
        case NOTE_E_FLAT:
        case NOTE_A_FLAT:
        case NOTE_D_FLAT:
        case NOTE_G_FLAT:
            return SCALE_FLAT;
        }
    }

    //MINOR SCALES
    switch(root)
    {
        //SHARP
    case NOTE_A:
    case NOTE_E:
    case NOTE_B:
    case NOTE_F_SHARP:
    case NOTE_C_SHARP:
    case NOTE_G_SHARP:
        return SCALE_SHARP;

        //FLAT
    case NOTE_D:
    case NOTE_G:
    case NOTE_C:
    case NOTE_F:
    case NOTE_B_FLAT:
    case NOTE_E_FLAT:
        return SCALE_FLAT;
    }

    printf("ERR: should never reach here\n");
}

void keys_draw_notes(keyboard_t *k, v2 pos, gl_charset_t *cset)
{
    int i,j, count, root, is_flat;
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
            if(count >= 20)
                break;

            if(k->keys[i] & (1 << j))
                notes[count++] = keys_get_note((i*8+j));
        }
    }

    notes_bm = keys_get_notes(notes, count);
    root = keys_root_note(notes_bm);

    if(count >= 2)
        is_flat = keys_sharp_or_flat(root, notes_bm);

    if(root != -1) {
        color.r = 255;
        pos = keys_render_note(k->rnote, root, is_flat, pos, color);
        color.r = 0;
    }

    for(i=0; i<count; i++)
    {
        newline = true;
        printf("%hhu ", notes[i]);
        pos = keys_render_note(k->rnote, notes[i], is_flat, pos, color);
    }

    if(newline)
      printf("\n");
}
