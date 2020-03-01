#include "sound.h"
#include "frite.h"

float midi_to_freq(uint8_t note)
{
    return pow(2,((float)note-69)/12) * 440.0;
}

void tone_init(tone_t *s, float freq)
{
    pback_settings_t pback = pback_settings();

    s->freq = freq;
    s->step = (freq/(float)pback.sample_rate);
    s->current_phase = 0.0;
    s->amplitude = 0.1; //Hardcode this for now. Here to prevent clipping.

    int format_bits = snd_pcm_format_width(pback.sample_fmt);
    s->maxval = (1 << (format_bits - 1)) - 1;
}

void sine_wave(tone_t *t, int16_t *buffer, uint16_t period)
{
    int i;

    for(i=0; i<period; i++)
    {
        buffer[i] += sin(MAX_PHASE*t->current_phase) * t->maxval * t->amplitude;
        t->current_phase += t->step;

        if(t->current_phase >= 1.0)
            t->current_phase -= 1.0;
    }
}

void tri_wave(tone_t *t, int16_t *buffer, uint16_t period)
{
    int i;

    for(i=0; i<period; i++)
    {
        if(t->current_phase <= 0.25)
        {
            buffer[i] += (4*t->current_phase) * t->maxval * t->amplitude;
        }
        else if(t->current_phase > 0.25 && t->current_phase <= 0.75)
        {
            buffer[i] += 2*(1-2*t->current_phase) * t->maxval * t->amplitude;
        }
        else if(t->current_phase >= 0.75)
        {
            buffer[i] += 4*(t->current_phase-1) * t->maxval * t->amplitude;
        }
        else
        {
            printf("WARN: tri_wave current_phase has value: %d\n", t->current_phase);
        }

        t->current_phase += t->step;

        if(t->current_phase >= 1.0)
            t->current_phase -= 1.0;
    }
}

void square_wave(tone_t *t, int16_t *buffer, uint16_t period)
{
    int i;

    for(i=0; i<period; i++)
    {
        if(t->current_phase <= 0.5)
        {
            buffer[i] += 0.5 * t->maxval * t->amplitude;
        }
        else
        {
            buffer[i] += -0.5 * t->maxval * t->amplitude;
        }

        t->current_phase += t->step;

        if(t->current_phase >= 1.0)
            t->current_phase -= 1.0;
    }
}
