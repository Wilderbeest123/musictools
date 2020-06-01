#include "frite.h"
#include "input.h"

static pback_settings_t *playback_settings;

pback_settings_t pback_settings()
{
    return *playback_settings;
}

int pback_maxval()
{
    pback_settings_t pback = pback_settings();
    int format_bits = snd_pcm_format_width(pback.sample_fmt);
    int maxval = (1 << (format_bits - 1)) - 1;
    return maxval;
}

void print_pback_settings(void)
{
    printf("Sample Rate: %lu samples/sec\n", playback_settings->sample_rate);
    printf("Buffer Time: %lu usec\n", playback_settings->buffer_time);
    printf("Period Time: %lu usec\n", playback_settings->period_time);
    printf("Buffer Size: %lu\n", playback_settings->buffer_size);
    printf("Period Size: %lu\n", playback_settings->period_size);
}

static void frite_print_hw_options(frite_t *hw)
{
    uint32_t val;

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

    snd_pcm_hw_params_get_period_time_min(hw->params_out, &val, 0);
    printf("Period Min Time: %lu\n", val);

    snd_pcm_hw_params_get_period_time_max(hw->params_out, &val, 0);
    printf("Period Max Time: %lu\n", val);

    snd_pcm_hw_params_get_buffer_time_min(hw->params_out, &val, 0);
    printf("Buffer Min Time: %lu\n", val);

    snd_pcm_hw_params_get_buffer_time_max(hw->params_out, &val, 0);
    printf("Buffer Max Time: %lu\n", val);
}

static void frite_midi_open(frite_t *hw, midi_events_t *m)
{
    char *midi_dev = "hw:2,0,0"; //This appears to not change
    int result = 0;

    //Midi In
    result = snd_rawmidi_open(&hw->midi_in, NULL, midi_dev, SND_RAWMIDI_TYPE_HW);

    if(result < 0)
        printf("Failed to open midi device handle");

    snd_rawmidi_nonblock(hw->midi_in, 1); //1 == NONBLOCK MODE

    //Set the MIDI event handle
    hw->midi_ev = m;
}

void frite_open(frite_t *hw, midi_events_t *m, bool midi_en)
{
    char *playback_dev = "default";
    int result = 0;

    if(midi_en)
        frite_midi_open(hw, m);
    else
        hw->midi_ev = NULL;

    //Audio Out
    hw->pback_out.sample_fmt = SND_PCM_FORMAT_S16;
    hw->pback_out.sample_rate = 44100;
    hw->pback_out.buffer_time = 200000;
    hw->pback_out.period_time = 50000;
    hw->out_buffer = calloc(hw->pback_out.period_size, sizeof(uint16_t));

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
    result = snd_pcm_hw_params_set_format(hw->audio_out, hw->params_out, hw->pback_out.sample_fmt);

    //Set Stream Rate
    result = snd_pcm_hw_params_set_rate_near(hw->audio_out,
                                             hw->params_out,
                                             &hw->pback_out.sample_rate, 0);

    //Set Buffer Time
    result = snd_pcm_hw_params_set_buffer_time_near(hw->audio_out, hw->params_out,
                                                    &hw->pback_out.buffer_time, &dir);

    //Get Buffer Size (In terms of frames)
    result = snd_pcm_hw_params_get_buffer_size(hw->params_out, &hw->pback_out.buffer_size);

    //Set Period Time
    result = snd_pcm_hw_params_set_period_time_near(hw->audio_out, hw->params_out,
                                                    &hw->pback_out.period_time, &dir);

    //Get Period Time (In terms of frames)
    result = snd_pcm_hw_params_get_period_size(hw->params_out, &hw->pback_out.period_size, &dir);

    //Write parameter to device
    result = snd_pcm_hw_params(hw->audio_out, hw->params_out);

    //Init playback settings static ptr
    playback_settings = &hw->pback_out;
}

input_flags_t frite_read(frite_t *hw)
{
    ssize_t size;
    uint8_t buf[256];
    input_flags_t flags;
    int i;

    size = snd_rawmidi_read(hw->midi_in,(void*)buf,256);
    flags = 0;

    if(size < 1)
        return 0;

    for(i=0;i<size;)
    {
        switch(buf[i])
        {
        case 0xfe: //
            i++;
            break;

        case 0x90:
            printf("ON: %hhx %hhx\n", buf[i+1], buf[i+2]);
            hw->midi_ev->press_on(hw->midi_ev, buf[i+1],buf[i+2]);
            flags |= INEVENT_MIDI_DOWN;
            i+=3;
            break;

        case 0x80:
            printf("OFF: %hhx %hhx\n", buf[i+1], buf[i+2]);
            hw->midi_ev->press_off(hw->midi_ev, buf[i+1],buf[i+2]);
            flags |= INEVENT_MIDI_UP;
            i+=3;
            break;

        case 0xb0:
            printf("PEDAL: %hhx %hhx\n", buf[i+1], buf[i+2]);
            flags |= INEVENT_MIDI_PEDAL;
            i+=3;
            break;

        default:
            printf("%hhx\n", buf[i]);
            i++;
            break;
        }
    }

    return flags;
}
