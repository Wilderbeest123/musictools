#ifndef KEYS_H_
#define KEYS_H_

#include "frite.h"
#include "sound.h"
#include "shapes.h"

typedef struct
{
  uint8_t keys[16]; //bitmask representing whether key is pressed
  midi_events_t ev;
  tone_t notes[12]; //Current notes being played
} keyboard_t;

void keys_init(keyboard_t *k);
void keys_print_notes(keyboard_t *k);
void keys_populate_buffer(keyboard_t *k, uint16_t *buffer, uint32_t size);

void keys_draw_notes(keyboard_t *k, v2 pos, gl_charset_t *cset);

#endif  /* KEYS_H_ */
