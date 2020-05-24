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

enum {
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
};

typedef enum {
    SCALE_SHARP=0,
    SCALE_FLAT=1,
} keys_scale_t ;

enum {
    CHORD_MIN=0,
    CHORD_MAJ=1,
};


void keys_init(keyboard_t *k);
void keys_print_notes(keyboard_t *k);
void keys_populate_buffer(keyboard_t *k, uint16_t *buffer, uint32_t size);

void keys_draw_notes(keyboard_t *k, v2 pos, gl_charset_t *cset);

render_notes_t render_notes_init(int fontsize);

#endif  /* KEYS_H_ */
