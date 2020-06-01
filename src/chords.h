#ifndef CHORDS_H_
#define CHORDS_H_

#include <stdint.h>

#include "jtime.h"
#include "input.h"
#include "ui.h"

typedef struct
{
    jtime_t timer;
    uint8_t state;
    input_t *in;

} chord_system_t;

typedef struct
{
    ui_node_t n;

    //Chord note data
    int rnote;                  /* Root note of chord */
    int is_flat;               /* If 0, notes considered sharp */
    uint8_t notes[20];          /* Buffer of notes pressed */
    int ncount;                 /* Number of notes pressed */
    uint16_t nmask;             /* The note mask */

} chord_ui_t;

enum {
    CHORD_STATE_IDLE,
    CHORD_STATE_PENDING,
    CHORD_STATE_TIMER,
    CHORD_STATE_APPEND
};

void chords_init(input_t *in);
void chords_update(void);

#endif  /* CHORDS_H_ */
