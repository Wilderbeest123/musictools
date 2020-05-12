#ifndef FRITE_H_
#define FRITE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "asoundlib.h"

#define CONTAINER_OF(ptr, type, member) ({                        \
            const typeof( ((type *)0)->member ) *__mptr = (ptr);  \
            (type *)( (char *)__mptr - offsetof(type,member) );})

typedef struct
{
    void (*press_on)(void*, uint8_t, uint8_t);
    void (*press_off)(void*, uint8_t, uint8_t);
    void (*pedal)(void*, uint8_t, uint8_t);
} midi_events_t;

typedef struct
{
    snd_pcm_format_t sample_fmt;
    uint32_t sample_rate;
    uint32_t buffer_time;
    uint32_t period_time;
    snd_pcm_uframes_t buffer_size;
    snd_pcm_uframes_t period_size;
} pback_settings_t;

typedef struct
{
    //playback
    snd_pcm_t *audio_out;
    snd_pcm_hw_params_t *params_out;
    pback_settings_t pback_out;
    uint16_t *out_buffer;

    //midi
    snd_rawmidi_t *midi_in;
    midi_events_t *midi_ev;
    bool midi_en;

} frite_t;

void print_pback_settings();
pback_settings_t pback_settings();
int pback_maxval();

void frite_open(frite_t *hw, midi_events_t *m, bool midi_en);
void frite_read(frite_t *hw);

#endif  /* FRITE_H_ */
