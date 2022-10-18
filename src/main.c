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
// fretboard_test_C - Test that system can determine
// an Open C Chord
//-------------------------
void fretboard_test_C(void) {
    fretboard_t f;

    // * ROOT note of Chord starts on the
    //   first string we strike.
    //
    f.str6 = -1;
    f.str5 = fretboard_find_note(NOTE_C, STR_A);
    f.str4 = fretboard_find_note(NOTE_E, STR_D);
    f.str3 = fretboard_find_note(NOTE_G, STR_G);
    f.str2 = fretboard_find_note(NOTE_C, STR_B);
    f.str1 = fretboard_find_note(NOTE_E, STR_E);
    printf("Open C Chord\n");
    fretboard_print_cli(f);
}

//-------------------------
// fretboard_test_A - Test that system can determine
// an Open A Chord
//-------------------------
void fretboard_test_A(void) {
    fretboard_t f;

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
    printf("Open A Chord\n");
    fretboard_print_cli(f);
}

//-------------------------
// fretboard_test_G - Test that system can determine
// an Open C Chord
//-------------------------
void fretboard_test_G(void) {
    fretboard_t f;

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
    printf("Open G Chord\n");
    fretboard_print_cli(f);
}

//-------------------------
// fretboard_test_E - Test that system can determine
// an Open E Chord
//-------------------------
void fretboard_test_E(void) {
    fretboard_t f;

    // * Root note E is the same as the fret position

    f.str6 = fretboard_find_note(NOTE_E, STR_E);
    f.str5 = fretboard_find_note(NOTE_B, STR_A);
    f.str4 = fretboard_find_note(NOTE_E, STR_D);
    f.str3 = fretboard_find_note(NOTE_G_SHARP, STR_G);
    f.str2 = fretboard_find_note(NOTE_B, STR_B);
    f.str1 = fretboard_find_note(NOTE_E, STR_E);
    printf("Open E Chord\n");
    fretboard_print_cli(f);
}

//-------------------------
// fretboard_test_D - Test that system can determine
// an Open D Chord
//-------------------------
void fretboard_test_D(void) {
    fretboard_t f;

    // * 2nd Inversion can be played on the first
    //   three strings of the guitar.

    f.str6 = -1;
    f.str5 = -1;
    f.str4 = fretboard_find_note(NOTE_D, STR_D);
    f.str3 = fretboard_find_note(NOTE_A, STR_G);
    f.str2 = fretboard_find_note(NOTE_D, STR_B);
    f.str1 = fretboard_find_note(NOTE_F_SHARP, STR_E);
    printf("Open D Chord\n");
    fretboard_print_cli(f);
}

typedef struct {
    gl_model_t s;
    uint32_t ftex; // Texture for the Fretboard
    uint32_t utex; // Uniform Texture for Fret Holdings
    v2 pos; // The base Render position
    gl_charset_t nmbrs; // Character set used to represent fret position

} fretboard_rend_t;

void fretboard_init(fretboard_rend_t *this)
{
    this->ftex = gl_load_image("res/fretboard.png");
    this->utex = gl_load_image("res/white.png");
    this->s = model_init_square();
    this->pos.x = 300;
    this->pos.y = 100;
    this->nmbrs = gl_load_charset("res/OpenSans-Bold.ttf", 32, '0', '9');
}

// This is a dummy function used to determine where
// to render the appropriate locations for the fretboard
void fretboard_draw(fretboard_rend_t *this)
{
    int xval = 33; // Offset for each note
    int yval = 20;
    v2 fsize = { 180, 120 }; // Total size of render
    v2 nsize = { 18, 18 }; // Size of each Fret Note
    v2 soff = { 16, 2 }; // The start point offset when rendering

    // Render the Base Fretboard
    glBindTexture(GL_TEXTURE_2D, this->ftex);
    square_draw(this->pos.x, this->pos.y, fsize.x, fsize.y, COLOR_INIT(255,255,255,255));

    // Render the Finger Holdings
    glBindTexture(GL_TEXTURE_2D, this->utex);

    for(int i=0; i<5; i++) {
        for(int j=0; j<6; j++) {
            circle_draw(this->pos.x + soff.x + (i*xval),
                        this->pos.y + soff.y + (j*yval),
                        nsize.x, nsize.y, COLOR_INIT(255,0,255,200));
        }
    }

    // Render the Fret Position
    gl_char_t c;

    c = charset_get_char(&this->nmbrs, '1');
    glBindTexture(GL_TEXTURE_2D, c.tid);
    square_draw(this->pos.x-25, this->pos.y + 95, c.size.x, c.size.y, COLOR_INIT(0,0,0,255));

    // In Case of a Second Number
    c = charset_get_char(&this->nmbrs, '2');
    glBindTexture(GL_TEXTURE_2D, c.tid);
    square_draw(this->pos.x-10, this->pos.y + 95, c.size.x, c.size.y, COLOR_INIT(0,0,0,255));
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

    fretboard_test_C();
    fretboard_test_A();
    fretboard_test_G();
    fretboard_test_E();
    fretboard_test_D();

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);

        // --- ENTER RENDER LOGIC HERE ---
        fretboard_draw(&f);

        model_uniform_color(COLOR_INIT(255, 0, 127, 255));
        screen_swap_buffer(&s);
    }
}

