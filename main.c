#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define  RMIDI_IMPLEMENTATION
#include "rmidi.h"

uint8_t* load_file(const char* filename, size_t* out)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    uint64_t size = ftell(f);
    rewind(f);

    uint8_t* buffer = (uint8_t*)RMIDI_MALLOC(size + 1);
    assert(buffer != NULL);
    size_t read = fread(buffer, 1, size, f);
    fclose(f);
    *out = read;
    return buffer;
}


void dump_midi(const Midi* midi)
{
    printf("Format %d, %u track(s), %u ticks per quarter\n",
           midi->format, midi->ntracks, midi->division);
    for (uint16_t i = 0; i < midi->ntracks; i++) {
        const MidiTrack* track = &midi->tracks[i];
        printf("Track %u: %zu event(s)\n", i, track->count);
        da_foreach(MidiEvent, ev, track) {
            printf("  tick %6u (+%4u) ", ev->tick, ev->delta);
            if (ev->kind == MIDI_EVENT_CHANNEL) {
                if (ev->as.channel.kind == MIDI_NOTE_ON) {
                    printf("NOTE_ON  ch%d key %d vel %d\n", ev->as.channel.as.note_on.channel,
                           ev->as.channel.as.note_on.note, ev->as.channel.as.note_on.velocity);
                } else {
                    printf("NOTE_OFF ch%d key %d vel %d\n", ev->as.channel.as.note_off.channel,
                           ev->as.channel.as.note_off.note, ev->as.channel.as.note_off.velocity);
                }
            } else if (ev->kind == MIDI_EVENT_META) {
                if (ev->as.meta.kind == MIDI_META_TEMPO) {
                    uint32_t us = ev->as.meta.as.tempo.us_per_quarter;
                    printf("TEMPO    %u us/quarter (%.1f BPM)\n", us, 60000000.0 / us);
                } else if (ev->as.meta.kind == MIDI_META_TIME_SIG) {
                    printf("TIME_SIG %d/%d clocks %d 32nds %d\n",
                           ev->as.meta.as.time_signature.numerator,
                           ev->as.meta.as.time_signature.denominator,
                           ev->as.meta.as.time_signature.clocks_per_click,
                           ev->as.meta.as.time_signature.notated_32nds);
                } else if (ev->as.meta.kind == MIDI_META_KEY_SIG) {
                    printf("KEY_SIG  %d %s, %s\n", ev->as.meta.as.key_signature.sharps_flats,
                           ev->as.meta.as.key_signature.sharps_flats < 0 ? "flat(s)" : "sharp(s)",
                           ev->as.meta.as.key_signature.is_minor ? "minor" : "major");
                }
            }
        }
    }
}

int main()
{
    size_t file_size;
    uint8_t* midi_buffer = load_file("test2.mid", &file_size);
    if (!midi_buffer) { printf("Could not open test2.mid\n"); return 1; }

    Midi midi = {0};
    if (!midi_parse(&midi, midi_buffer, file_size)) {
        printf("Not a supported MIDI file\n");
        midi_free(&midi);
        free(midi_buffer);
        return 1;
    }
    dump_midi(&midi);
    midi_free(&midi);
    free(midi_buffer);
    return 0;
}
