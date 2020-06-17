#ifndef KEYS_H_
#define KEYS_H_

#include "frite.h"
#include "sound.h"
#include "shapes.h"

typedef struct
{
    gl_charset_t cset;          /* Letters used for rendering notes */
    uint32_t s_tex;             /* Sharp note texure  */
    v2 s_off;                   /* Sharp texture pos offset */
    uint32_t f_tex;             /* Flat note texure  */
    v2 f_off;                   /* Flat texture pos offset  */
    int fs_size;                /* Flat/Sharp texture size */

} render_notes_t;

typedef struct
{
    uint8_t keys[16]; //bitmask representing whether key is pressed
    midi_events_t ev;
    tone_t notes[12]; //Current notes being played

    render_notes_t rnote;
} keyboard_t;

typedef enum {
    NOTE_C=0,
    NOTE_C_SHARP=1,
    NOTE_D_FLAT=1,
    NOTE_D=2,
    NOTE_D_SHARP=3,
    NOTE_E_FLAT=3,
    NOTE_E=4,
    NOTE_E_SHARP=5,
    NOTE_F=5,
    NOTE_F_SHARP=6,
    NOTE_G_FLAT=6,
    NOTE_G=7,
    NOTE_G_SHARP=8,
    NOTE_A_FLAT=8,
    NOTE_A=9,
    NOTE_A_SHARP=10,
    NOTE_B_FLAT=10,
    NOTE_B=11,
    NOTE_C_FLAT=11
} keys_note_t;

typedef enum {
    SCALE_SHARP=0,
    SCALE_FLAT=1,
} keys_scale_t;

enum {
    CHORD_MIN=0,
    CHORD_MAJ=1,
};

void keys_init(keyboard_t *k);
void keys_print_notes(keyboard_t *k);
void keys_populate_buffer(keyboard_t *k, uint16_t *buffer, uint32_t size);

void keys_draw_notes(keyboard_t *k, v2 pos, gl_charset_t *cset);
int keys_get_notes(uint8_t *buf, int bufsize);
int keys_get_root_by_thirds(uint16_t notes);
uint16_t keys_get_note_mask(uint8_t* data, int num);

v2 keys_render_size(uint8_t *buf, int bsize,
                    keys_scale_t scale, render_notes_t *r);

void keys_render_notes(v2 pos, uint8_t *buf, int bsize,
                       keys_scale_t scale, render_notes_t *r);

int keys_sharp_or_flat(int root, uint16_t notes);

render_notes_t render_notes_init(int fontsize);

#endif  /* KEYS_H_ */
