#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "jtime.h"
#include "frite.h"
#include "sound.h"
#include "asoundlib.h"
#include "keys.h"
#include "screen.h"
#include "shapes.h"
#include "input.h"
#include "box.h"
#include "button.h"
#include "ui.h"
#include "toolbar.h"
#include "wform.h"
#include "textbox.h"
#include "chords.h"

//-------------------------
// fretboard_tune_t - This represents the current tuning of
// the Guitar which is being played.
//-------------------------
typedef struct {
    uint8_t str1;
    uint8_t str2;
    uint8_t str3;
    uint8_t str4;
    uint8_t str5;
    uint8_t str6;
} fretboard_tune_t;

fretboard_tune_t std_tuning = { .str1=NOTE_E, .str2=NOTE_B, .str3=NOTE_G,
                                .str4=NOTE_D, .str5=NOTE_A, .str6=NOTE_E };

typedef enum {
    STR_HIGH_E=1,
    STR_B=2,
    STR_G=3,
    STR_D=4,
    STR_A=5,
    STR_E=6
} std_tuning_string;

//-------------------------
// fretboard_t - This is a programming representation of a
// fretboard. The data stored needs to be translated based
// upon the tuning of the Guitar itself.
//
// A value of 0 represents playing an open string.
//
// A value of -1 represents the string is being blocked.
//
// A value greater than 0 represents a fret being played
//-------------------------
typedef struct {
    keys_note_t str1; // High E String
    keys_note_t str2; // B String
    keys_note_t str3; // G String
    keys_note_t str4; // D String
    keys_note_t str5; // A String
    keys_note_t str6; // E String
} fretboard_t;

//-------------------------
// fretboard_find_note - Find a note on a
// particular string on the guitar
// note - The note you wish to play
//
//-------------------------
uint8_t fretboard_find_note(keys_note_t note, uint8_t string) {
    keys_note_t open_str;

    switch (string) {
    case(1): open_str = std_tuning.str1; break;
    case(2): open_str = std_tuning.str2; break;
    case(3): open_str = std_tuning.str3; break;
    case(4): open_str = std_tuning.str4; break;
    case(5): open_str = std_tuning.str5; break;
    case(6): open_str = std_tuning.str6; break;
    default:
        printf("ERR: fretboard_find_note - invalid value\n");
        return 0;
    }

    // Determine difference in semitones
    if(note < open_str) {
        // Need to account for difference
        return NOTE_OCTAVE - open_str + note;
    }

    return note - open_str;
}

//-------------------------
// fretboard_print_cli - Print contents
// of a fretboard onto the command line
//-------------------------
void fretboard_print_cli(fretboard_t f) {
    printf("-------------------\n");
    printf("%u\n", f.str1);
    printf("%u\n", f.str2);
    printf("%u\n", f.str3);
    printf("%u\n", f.str4);
    printf("%u\n", f.str5);
    printf("%u\n", f.str6);
    printf("-------------------\n");
}

//-------------------------
// fretboard_rend_fingers_t - This provides the necessary
// offset information as to how to render the correct
// shape to represent the data stored in fretboard_t
// 0xFF == String Not Played
// 0x00 == Fret Position
// 0x01 == Render Fret+1 Position
// 0x02 == Render Fret+2 Position
// etc.
//-------------------------
typedef struct {
    uint8_t str1; // High E String
    uint8_t str2; // B String
    uint8_t str3; // G String
    uint8_t str4; // D String
    uint8_t str5; // A String
    uint8_t str6; // E String

} fretboard_rend_fingers_t;

//-------------------------
// fretboard_rend_print_cli - Print contents
// of a fretboard onto the command line
//-------------------------
void fretboard_rend_print_cli(fretboard_rend_fingers_t f) {
    printf("-------------------\n");
    printf("%u\n", f.str1);
    printf("%u\n", f.str2);
    printf("%u\n", f.str3);
    printf("%u\n", f.str4);
    printf("%u\n", f.str5);
    printf("%u\n", f.str6);
    printf("-------------------\n");
}

fretboard_rend_fingers_t fretboard_get_render_positions(fretboard_t *f, uint8_t start_fret) {
    uint8_t llim, hlim; // Low & High Limits
    fretboard_rend_fingers_t rend;

    rend.str1 = f->str1 - start_fret;
    rend.str2 = f->str2 - start_fret;
    rend.str3 = f->str3 - start_fret;
    rend.str4 = f->str4 - start_fret;
    rend.str5 = f->str5 - start_fret;
    rend.str6 = f->str6 - start_fret;

    // Determine if each string is in appropriate bounds
    // otherwise set to 0xFF
    llim = start_fret;
    hlim = start_fret + 6; // Six possible positions on the fret

    if(rend.str1 < llim || rend.str1 > hlim)
        rend.str1 = 0xFF;

    if(rend.str2 < llim || rend.str2 > hlim)
        rend.str2 = 0xFF;

    if(rend.str3 < llim || rend.str3 > hlim)
        rend.str3 = 0xFF;

    if(rend.str4 < llim || rend.str4 > hlim)
        rend.str4 = 0xFF;

    if(rend.str5 < llim || rend.str5 > hlim)
        rend.str5 = 0xFF;

    if(rend.str6 < llim || rend.str6 > hlim)
        rend.str6 = 0xFF;

    return rend;
}

//-------------------------
// fretboard_rend_t - This struct contains all
// of the data responsible for rendering the
// fretboard and the notes being played
//-------------------------
typedef struct {
    gl_model_t s;
    uint32_t ftex; // Texture for the Fretboard
    uint32_t utex; // Uniform Texture for Fret Holdings
    v2 pos; // The base Render position
    gl_charset_t nmbrs; // Character set used to represent fret position

    v2 rsize; // Size of the Fretboard
    v2 nsize; // Size of each played Note
    v2 nstoff; // Start point Render offset
    v2 nval; // Seperation values between each Rendered Note

    v2 nrendoff; // The Note Render Offset relative to the Note Drawn
    render_notes_t nrend; // Data for rendering notes

} fretboard_rend_t;

void fretboard_init(fretboard_rend_t *this)
{
    this->pos.x = 300;
    this->pos.y = 100;

    this->ftex = gl_load_image("res/fretboard.png");
    this->utex = gl_load_image("res/white.png");
    this->s = model_init_square();
    this->nmbrs = gl_load_charset("res/OpenSans-Bold.ttf", 32, '0', '9');

    this->rsize = V2(180, 120);
    this->nsize = V2(18, 18);
    this->nstoff = V2(16, 2);
    this->nval = V2(33, 20);

    this->nrendoff = V2(2,-4);
    this->nrend = render_notes_init(18);
}

//-------------------------
// fretboard_rend_finger_note - Render the Note being played on
// top of the fretboard pressed note
//-------------------------
keys_note_t fretboard_note_played(fretboard_tune_t t, uint8_t strid, uint8_t fret_pressed) {
    uint8_t start_note;
    keys_note_t note_played;

    start_note = *((uint8_t*)(&t.str1+strid));
    note_played = start_note + fret_pressed;

    if(note_played >= NOTE_OCTAVE)
        note_played = note_played - NOTE_OCTAVE;

    return note_played;
}

// This is a test function used to determine where
// to render the appropriate locations for the fretboard
void fretboard_rend_test(fretboard_rend_t *this)
{
    gl_char_t c;
    keys_note_t n;
    v2 pos;

    // Render the Base Fretboard
    glBindTexture(GL_TEXTURE_2D, this->ftex);
    square_draw(this->pos.x, this->pos.y, this->rsize.x, this->rsize.y, COLOR_INIT(255,255,255,255));

    // Render the 'Open' Fret Positions
    for(int i=0; i<6; i++) {
        pos.x = this->pos.x - 10;
        pos.y = this->pos.y + this->nstoff.y + (i*this->nval.y);

        pos.x += this->nrendoff.x;
        pos.y += this->nrendoff.y;

        n = fretboard_note_played(std_tuning, i, 0);
        keys_render_note(this->nrend, n, SCALE_FLAT, pos, COLOR_INIT(0,0,0,255));
    }

    // Render the Finger Holdings
    glBindTexture(GL_TEXTURE_2D, this->utex);

    for(int i=0; i<5; i++) {
        for(int j=0; j<6; j++) {
            pos.x = this->pos.x + this->nstoff.x + (i*this->nval.x);
            pos.y = this->pos.y + this->nstoff.y + (j*this->nval.y);

            glBindTexture(GL_TEXTURE_2D, this->utex);
            circle_draw(pos.x, pos.y,
                        this->nsize.x, this->nsize.y, COLOR_INIT(255,0,255,200));

            pos.x += this->nrendoff.x;
            pos.y += this->nrendoff.y;

            // Render what note is being played
            n = fretboard_note_played(std_tuning, j, i+1);
            keys_render_note(this->nrend, n, SCALE_FLAT, pos, COLOR_INIT(0,0,0,255));
        }
    }


    // This is for rendering 'one' number
    c = charset_get_char(&this->nmbrs, '0');
    glBindTexture(GL_TEXTURE_2D, c.tid);
    square_draw(this->pos.x-10, this->pos.y + 110, c.size.x, c.size.y, COLOR_INIT(0,0,0,255));

    // This is for rendering 'two' numbers
    if(0) {
        // Render the Fret Position
        c = charset_get_char(&this->nmbrs, '1');
        glBindTexture(GL_TEXTURE_2D, c.tid);
        square_draw(this->pos.x-20, this->pos.y + 110, c.size.x, c.size.y, COLOR_INIT(0,0,0,255));

        // In Case of a Second Number
        c = charset_get_char(&this->nmbrs, '2');
        glBindTexture(GL_TEXTURE_2D, c.tid);
        square_draw(this->pos.x-5, this->pos.y + 110, c.size.x, c.size.y, COLOR_INIT(0,0,0,255));
    }

    // Render the place you Bar the Chord
    glBindTexture(GL_TEXTURE_2D, this->utex);
    square_draw(this->pos.x+5, this->pos.y+10, 5, 100, COLOR_INIT(255,0,255,200));
    return;
}

//-------------------------
// fretboard_render - Render a Fretboard based upon
// the contents stored within the fretboard_t
//-------------------------
void fretboard_render(fretboard_rend_t *this, fretboard_t f, uint8_t start_fret) {
    v2 rpos;
    uint8_t *sptr;
    fretboard_rend_fingers_t frend;
    gl_char_t c;

    frend = fretboard_get_render_positions(&f, start_fret);

    // Render the Base Fretboard
    glBindTexture(GL_TEXTURE_2D, this->ftex);
    square_draw(this->pos.x, this->pos.y, this->rsize.x, this->rsize.y, COLOR_INIT(255,255,255,255));

    // Render the Finger Holdings
    glBindTexture(GL_TEXTURE_2D, this->utex);

    sptr = &frend.str1;
    for(int j=0; j<6; j++) {

        if(*sptr == 0xFF || *sptr == 0x00) {
            sptr++;
            continue;
        }

        rpos.x = this->pos.x + this->nstoff.x + ((*sptr - 1) * this->nval.x);
        rpos.y = this->pos.y + this->nstoff.y + (j*this->nval.y);
        circle_draw(rpos.x, rpos.y, this->nsize.x, this->nsize.y, COLOR_INIT(255,0,255,200));
        sptr++;
    }

    // Render the Fret Position
    c = charset_get_char(&this->nmbrs, '0');
    glBindTexture(GL_TEXTURE_2D, c.tid);
    square_draw(this->pos.x-10, this->pos.y + 100, c.size.x, c.size.y, COLOR_INIT(0,0,0,255));
}

//-------------------------
// fretboard_test_C - Test that system can determine
// an Open C Chord
//-------------------------
void fretboard_test_C(fretboard_rend_t *frend) {
    fretboard_t f;
    fretboard_rend_fingers_t rend;

    // * ROOT note of Chord starts on the
    //   first string we strike.
    //
    f.str6 = -1;
    f.str5 = fretboard_find_note(NOTE_C, STR_A);
    f.str4 = fretboard_find_note(NOTE_E, STR_D);
    f.str3 = fretboard_find_note(NOTE_G, STR_G);
    f.str2 = fretboard_find_note(NOTE_C, STR_B);
    f.str1 = fretboard_find_note(NOTE_E, STR_E);
    //printf("Open C Chord\n");
    //fretboard_print_cli(f);
    fretboard_render(frend, f, 0);
}

//-------------------------
// fretboard_test_A - Test that system can determine
// an Open A Chord
//-------------------------
void fretboard_test_A(fretboard_rend_t *frend) {
    fretboard_t f;
    fretboard_rend_fingers_t rend;

    // * ROOT note of the chord begins with the
    //   third string up
    //
    // * A 2nd inversion can be played starting
    //   on the 4th string up
    f.str6 = -1;
    f.str5 = fretboard_find_note(NOTE_A, STR_A);
    f.str4 = fretboard_find_note(NOTE_E, STR_D);
    f.str3 = fretboard_find_note(NOTE_A, STR_G);
    f.str2 = fretboard_find_note(NOTE_C_SHARP, STR_B);
    f.str1 = fretboard_find_note(NOTE_E, STR_E);
    //printf("Open A Chord\n");
    //fretboard_print_cli(f);
    fretboard_render(frend, f, 0);
}

//-------------------------
// fretboard_test_G - Test that system can determine
// an Open C Chord
//-------------------------
void fretboard_test_G(fretboard_rend_t *frend) {
    fretboard_t f;
    fretboard_rend_fingers_t rend;

    // * The ROOT Triad can be played on the 1st
    //   string we strike
    //
    // * 1st Inversion can be played on the 2nd string
    f.str6 = fretboard_find_note(NOTE_G, STR_E);
    f.str5 = fretboard_find_note(NOTE_B, STR_A);
    f.str4 = fretboard_find_note(NOTE_D, STR_D);
    f.str3 = fretboard_find_note(NOTE_G, STR_G);
    f.str2 = fretboard_find_note(NOTE_B, STR_B);
    f.str1 = fretboard_find_note(NOTE_G, STR_E);
    //printf("Open G Chord\n");
    //fretboard_print_cli(f);
    fretboard_render(frend, f, 0);
}

//-------------------------
// fretboard_test_E - Test that system can determine
// an Open E Chord
//-------------------------
void fretboard_test_E(fretboard_rend_t *frend) {
    fretboard_t f;
    fretboard_rend_fingers_t rend;

    // * Root note E is the same as the fret position
    f.str6 = fretboard_find_note(NOTE_E, STR_E);
    f.str5 = fretboard_find_note(NOTE_B, STR_A);
    f.str4 = fretboard_find_note(NOTE_E, STR_D);
    f.str3 = fretboard_find_note(NOTE_G_SHARP, STR_G);
    f.str2 = fretboard_find_note(NOTE_B, STR_B);
    f.str1 = fretboard_find_note(NOTE_E, STR_E);
    //printf("Open E Chord\n");
    //fretboard_print_cli(f);
    fretboard_render(frend, f, 0);
}

//-------------------------
// fretboard_test_D - Test that system can determine
// an Open D Chord
//-------------------------
void fretboard_test_D(fretboard_rend_t *frend) {
    fretboard_t f;
    fretboard_rend_fingers_t rend;

    // * 2nd Inversion can be played on the first
    //   three strings of the guitar.
    //
    // * The first note we play is the ROOT Note
    //
    // * Root note D is the same as the fret postition

    f.str6 = -1;
    f.str5 = -1;
    f.str4 = fretboard_find_note(NOTE_D, STR_D);
    f.str3 = fretboard_find_note(NOTE_A, STR_G);
    f.str2 = fretboard_find_note(NOTE_D, STR_B);
    f.str1 = fretboard_find_note(NOTE_F_SHARP, STR_E);
    //printf("Open D Chord\n");
    //fretboard_print_cli(f);
    fretboard_render(frend, f, 0);
}

int main(void)
{
    bool frite_midi = false;

    //Screen/UI
    screen_t s;
    input_t in;
    shapes_t sh;
    ui_system_t uisys;

    // Sound
    frite_t hw;
    keyboard_t k;

    //Screen/UI init
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s, &hw, !frite_midi);
    shapes_init(&sh);
    uisys_init(&uisys, &in);

    fretboard_rend_t f;
    fretboard_init(&f);

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);

        // --- ENTER RENDER LOGIC HERE ---
        //fretboard_test_C(&f);
        fretboard_rend_test(&f);

        model_uniform_color(COLOR_INIT(255, 0, 127, 255));
        screen_swap_buffer(&s);
    }
}

