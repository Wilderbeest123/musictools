
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "jtime.h"
#include "asoundlib.h"

typedef struct
{
  uint8_t keys[16]; //bitmask representing whether key is pressed;
} keyboard_t;

void keys_init(keyboard_t *k)
{
  int i;

  for(i=0;i<16;i++)
    k->keys[i] = 0;
}

void keys_midi_handle_on(keyboard_t *k, uint8_t note, uint8_t velocity)
{
  k->keys[note/8] |= (1 << note%8);
}

void keys_midi_handle_off(keyboard_t *k, uint8_t note, uint8_t velocity)
{
  k->keys[note/8] &= ~(1 << note%8);
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

typedef struct
{
    snd_rawmidi_t *midi_in;
    snd_pcm_t *audio_out;
    snd_pcm_hw_params_t *params_out;
    snd_pcm_format_t sample_fmt;
    uint32_t sample_rate;
    uint32_t buffer_time;
    uint32_t period_time;
    snd_pcm_uframes_t buffer_size;
    snd_pcm_uframes_t period_size;
} frite_t;

void frite_print_hw_options(frite_t *hw)
{

    printf("Supported Memory Access Types: ");

    if(snd_pcm_hw_params_test_access(hw->audio_out, hw->params_out, SND_PCM_ACCESS_MMAP_INTERLEAVED) == 0)
        printf("MMAP_INTERLEAVED ");

    if(snd_pcm_hw_params_test_access(hw->audio_out, hw->params_out, SND_PCM_ACCESS_MMAP_NONINTERLEAVED) == 0)
        printf("MMAP_NONINTERLEAVED ");

    if(snd_pcm_hw_params_test_access(hw->audio_out, hw->params_out, SND_PCM_ACCESS_MMAP_COMPLEX) == 0)
        printf("MMAP_COMPLEX ");

    if(snd_pcm_hw_params_test_access(hw->audio_out, hw->params_out, SND_PCM_ACCESS_RW_INTERLEAVED) == 0)
        printf("RW_INTERLEAVED ");

    if(snd_pcm_hw_params_test_access(hw->audio_out, hw->params_out, SND_PCM_ACCESS_RW_NONINTERLEAVED) == 0)
        printf("RW_NONINTERLEAVED ");

    printf("\n");
}

void frite_open(frite_t *hw)
{

    char *midi_dev = "hw:2,0,0"; //This appears to not change
    char *playback_dev = "default";
    int result = 0;


    //Midi In
    result = snd_rawmidi_open(&hw->midi_in, NULL, midi_dev, SND_RAWMIDI_TYPE_HW);

    if(result < 0)
        printf("Failed to open midi device hanlde");

    snd_rawmidi_nonblock(hw->midi_in, 1); //1 == NONBLOCK MODE

    //Audio Out
    hw->sample_fmt = SND_PCM_FORMAT_S16;
    hw->sample_rate = 44100;
    hw->buffer_time = 500000;
    hw->period_time = 100000;

    int dir; //Uncertain why required, keep here for now

    snd_pcm_hw_params_alloca(&hw->params_out);

    result = snd_pcm_open(&hw->audio_out, playback_dev, SND_PCM_STREAM_PLAYBACK, 0);

    //Init available options based on hardware
    result = snd_pcm_hw_params_any(hw->audio_out, hw->params_out);

    frite_print_hw_options(hw);

    //Hardware resampling
    result = snd_pcm_hw_params_set_rate_resample(hw->audio_out, hw->params_out, 1);

    //Configure Memory Access
    result = snd_pcm_hw_params_set_access(hw->audio_out, hw->params_out,
                                          SND_PCM_ACCESS_RW_INTERLEAVED);

    //Set Output Format
    result = snd_pcm_hw_params_set_format(hw->audio_out, hw->params_out, hw->sample_fmt);

    //Set Stream Rate
    result = snd_pcm_hw_params_set_rate_near(hw->audio_out,
                                             hw->params_out,
                                             &hw->sample_rate, 0);

    //Set Buffer Time
    result = snd_pcm_hw_params_set_buffer_time_near(hw->audio_out, hw->params_out,
                                                    &hw->buffer_time, &dir);

    //Get Buffer Size (In terms of frames)
    result = snd_pcm_hw_params_get_buffer_size(hw->params_out, &hw->buffer_size);

    //Set Period Time
    result = snd_pcm_hw_params_set_period_time_near(hw->audio_out, hw->params_out, &hw->period_time, &dir);

    //Get Period Time (In terms of frames)
    result = snd_pcm_hw_params_get_period_size(hw->params_out, &hw->period_size, &dir);

    //Write parameter to device
    result = snd_pcm_hw_params(hw->audio_out, hw->params_out);
}

void frite_print_settings(frite_t *hw)
{
    printf("Sample Rate: %lu samples/sec\n", hw->sample_rate);
    printf("Buffer Time: %lu usec\n", hw->buffer_time);
    printf("Period Time: %lu usec\n", hw->period_time);
    printf("Buffer Size: %lu\n", hw->buffer_size);
    printf("Period Size: %lu\n", hw->period_size);
}

void frite_read(frite_t *hw, keyboard_t *k)
{
    ssize_t size;
    uint8_t buf[256];
    int i;

    size = snd_rawmidi_read(hw->midi_in,(void*)buf,256);

    if(size < 1)
        return;

    for(i=0;i<size;)
    {
        switch(buf[i])
        {
        case 0xfe: //
            i++;
            break;

        case 0x90:
            printf("ON: %hhx %hhx\n", buf[i+1], buf[i+2]);
            keys_midi_handle_on(k, buf[i+1], buf[i+2]);
            i+=3;
            break;

        case 0x80:
            printf("OFF: %hhx %hhx\n", buf[i+1], buf[i+2]);
            keys_midi_handle_off(k, buf[i+1], buf[i+2]);
            i+=3;
            break;

        case 0xb0:
            printf("PEDAL: %hhx %hhx\n", buf[i+1], buf[i+2]);
            i+=3;
            break;

        default:
            printf("%hhx\n", buf[i]);
            i++;
            break;
        }
    }
}

typedef struct
{
    int freq;
    double step;
    double current_phase;
    uint16_t maxval;
} sinewave_t;

void sine_init(sinewave_t *s, frite_t *hw, int freq)
{
    s->freq = freq;
    s->step = (M_PI*2)*((float)freq/hw->sample_rate);
    s->current_phase = 0.0;

    int format_bits = snd_pcm_format_width(hw->sample_fmt);
    s->maxval = (1 << (format_bits - 1)) - 1;
}

void sine_wave(sinewave_t *s, int16_t *buffer, uint16_t period)
{
    double max_phase = M_PI*2;
    int i;

    for(i=0; i<period; i++)
    {
        buffer[i] = sin(s->current_phase) * s->maxval;

        s->current_phase += s->step;

        if(s->current_phase >= max_phase)
            s->current_phase -= max_phase;
    }
}

int main(void)
{
    frite_t hw;
    jtime_t timer;
    keyboard_t k;
    sinewave_t s;
    int16_t *period_buffer;

    keys_init(&k);
    frite_open(&hw);
    frite_print_settings(&hw);

    period_buffer = malloc(sizeof(int16_t)*hw.period_size);
    sine_init(&s, &hw, 100);
    sine_wave(&s, period_buffer, hw.period_size);

    timer_init(&timer, 20);

    while(1)
    {
        if(timer_check(&timer)){
            //print_time();
            frite_read(&hw, &k);
            keys_print_notes(&k);

            //Only write if space availabe in audio buffer.
            if(snd_pcm_avail(hw.audio_out) > hw.period_size)
            {
                snd_pcm_writei(hw.audio_out, period_buffer, hw.period_size);
                sine_wave(&s, period_buffer, hw.period_size);
            }
        }
    }

    free(period_buffer);
    snd_rawmidi_drain(hw.midi_in);
    snd_rawmidi_close(hw.midi_in);
    return 0;
}
