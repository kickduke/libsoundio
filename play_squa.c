/*gcc -o play_squa play_squa.c -lsoundio -lm*/

#include <soundio/soundio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
static float seconds_offset = 0.0f;
int sound_length = 0;

//方波
static void write_callback(struct SoundIoOutStream *outstream,
        int frame_count_min, int frame_count_max)
{
    const struct SoundIoChannelLayout *layout = &outstream->layout;
    float const PCT = 12.5f;
    float vol = 1.0f;
    float amp = 1.0f;     //振幅0.0-1.0
    float float_sample_rate = outstream->sample_rate;
    float seconds_per_frame = 1.0f / float_sample_rate;
    struct SoundIoChannelArea *areas;
    int frames_left = frame_count_max;
    int bytes_per_frame = outstream->bytes_per_frame;
    float sample = 0;
    while (frames_left > 0) {
        int frame_count = frames_left;
        soundio_outstream_begin_write(outstream, &areas, &frame_count);
        if (!frame_count)
            break;
        float pitch;
        if(sound_length < 10) {
            pitch = 440.0f;
        }else {
            pitch = 493.88f;
        }
        float radians_per_second = pitch * 8.0f;    //假设每个方波长是8
        for (int frame = 0; frame < frame_count; frame += 1) {
            //float sample = sinf((seconds_offset + frame * seconds_per_frame) * radians_per_second);
            //fprintf(stderr, "test:%f %f\n", (seconds_offset + frame * seconds_per_frame) * radians_per_second, PCT/100.0f*8.0f);
            //fprintf(stderr, "test2:%f\n", fmod((seconds_offset + frame * seconds_per_frame) * radians_per_second, 8) );
            //fprintf(stderr, "test2:%f %ld\n", vol, amp);
            if(fmod((seconds_offset + frame * seconds_per_frame) * radians_per_second, 8) <= PCT/100.0f*8.0f) {
                sample = vol * amp;
                fprintf(stderr, "sample:%e\n", sample);
            } else {
                sample = 0;
            }
            for (int channel = 0; channel < layout->channel_count; channel += 1) {
                float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);   //step=bytes_per_frame
                *ptr = sample;
            }
        }
        seconds_offset += seconds_per_frame * frame_count;
        soundio_outstream_end_write(outstream);
        frames_left -= frame_count;
        sound_length++;
    }
}
int main(int argc, char **argv) {
    int err;
    struct SoundIo *soundio = soundio_create();
    soundio_connect(soundio);
    soundio_flush_events(soundio);
    int default_out_device_index = soundio_default_output_device_index(soundio);
    struct SoundIoDevice *device = soundio_get_output_device(soundio, default_out_device_index);
    fprintf(stderr, "Output device: %s\n", device->name);
    struct SoundIoOutStream *outstream = soundio_outstream_create(device);
    outstream->format = SoundIoFormatFloat32NE;    
    outstream->write_callback = write_callback;
    soundio_outstream_open(outstream);
    if (outstream->layout_error)
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(outstream->layout_error));
    soundio_outstream_start(outstream);
    //测试outstream相关参数
    fprintf(stderr, "sample_rate:%d\n", outstream->sample_rate);
    fprintf(stderr, "bytes_per_frame:%d\n", outstream->bytes_per_frame);
    fprintf(stderr, "channel_count:%d\n", outstream->layout.channel_count);
    //
    for (;;)
        soundio_wait_events(soundio);
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
    return 0;
}