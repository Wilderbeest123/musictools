#include "sound.h"
#include "frite.h"

float midi_to_freq(uint8_t note)
{
    return pow(2,((float)note-69)/12) * 440.0;
}

void sine_init(sinewave_t *s, float freq)
{
    pback_settings_t pback = pback_settings();

    s->freq = freq;
    s->step = MAX_PHASE*(freq/(float)pback.sample_rate);
    s->current_phase = 0.0;
    s->amplitude = 0.1; //Hardcode this for now. Here to prevent clipping.

    int format_bits = snd_pcm_format_width(pback.sample_fmt);
    s->maxval = (1 << (format_bits - 1)) - 1;
}

void sine_wave(sinewave_t *s, int16_t *buffer, uint16_t period)
{
    int i;

    for(i=0; i<period; i++)
    {
        buffer[i] += sin(s->current_phase) * s->maxval * s->amplitude;
        s->current_phase += s->step;

        if(s->current_phase >= MAX_PHASE)
            s->current_phase -= MAX_PHASE;
    }
}

void mix_output(int16_t *out, int16_t *b1, int16_t *b2, uint16_t period)
{
    int i;

    for(i=0; i<period; i++)
        out[i] = b1[i] + b2[i];
}
