/*
                             @@....--.......@@@                      
                          @@@.....-............@@                    
                       @...---.....................@                 
                    @........-##..-...............   @@              
                  @........++#++....--.......    ........@           
               @@.........++++++.........     ...........--@         
               @@.    ...++===++..   ..............--.------@        
                @.......+++==++..  ....--..........----------@       
                @--..---#++++++......--------------------+---  @  @  
               @--------##+++##---..-------##---##--+----++- ** ** @ 
             @@---------#######--------+-----###-----+----- ******* @
           @@%%---------#######------------++++-------+-----+ *** @  
         @##%-----------########+---------------+-----+...---+- @    
        @##%%------+----+########+----------....-+-----..----+---@   
      @#-##%#-----+----++--#######-+----....%%%.-+-----+++-------@   
      @##-###-----+----++-----####---+--%%%%....-+-----++++---+--@   
         @###@@--+++----%%%%%%%%%%...-...........-----+++++---+--@   
             @@+@@-+----++-   ++++..............-----++++++--+++--@  
               @+@+------++..  ===.............-----++++++---+++--@  
                @+++---------.................----+++++--+---++++-@  
                @++++-----------............---##+++++---+----++++-@ 
                @++++--+--+++++++.......---####--++@@--+++----@   @  
                 @++++----+++++++++++++####--..........@@@@+---@     
                   @++++--+@@  @@@......###.................@--@     
                      @++@@@@.........######............. ...@-@     
                       @@@..........#########.................@      
                        @..........###########................@      
                       @...........--.########.....-...........@     
                      @...........-..##########....-@-..........@    
                    @....--....--..-###########...--@ @..........@   
                   @......-==######%%############.-@   @@.........@  
                 @.......-=#######%%%#############-@  @..........@   
                @......--#########%%#############%-@ @.........-@    
               @......--.#%%%#####%%%###########%%@@..........-@     

Copyright (c) 2026 UnknownRori
Licensed under the PolyForm Noncommercial License 1.0.0
See LICENSE file in repository root for full terms.
 */

#pragma once

#ifndef RMIDI_H
#define RMIDI_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define RMIDI_MALLOC(X)     malloc(X)
#define RMIDI_REALLOC(P, X) realloc(P, X)
#define RMIDI_FREE(X)       free(X)

#define RSTB_DA_REALLOC     RMIDI_REALLOC
#define RSTB_DA_FREE        RMIDI_FREE
#define RSTB_DA_H_STRIP_PREFIX
#include <rstb_da.h>

#define MAX_CHANNEL     16

#pragma pack(push, 1)
typedef struct RawChunkHeader {
    char     tag[4];
    uint32_t size;
} RawChunkHeader;

typedef struct RawMidiHeader {
    RawChunkHeader header;
    uint16_t       format;
    uint16_t       ntracks;
    uint16_t       div;
} RawMidiHeader;
#pragma pack(pop)

_Static_assert(sizeof(RawChunkHeader) == 8,  "RawChunkHeader must be 8 bytes");
_Static_assert(sizeof(RawMidiHeader)  == 14, "RawMidiHeader must be 14 bytes");

static inline uint16_t be16(uint16_t v) {
    const uint8_t *b = (const uint8_t *)&v;
    return (uint16_t)((b[0] << 8) | b[1]);
}
static inline uint32_t be32(uint32_t v) {
    const uint8_t *b = (const uint8_t *)&v;
    return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
           ((uint32_t)b[2] << 8)  |  (uint32_t)b[3];
}

typedef enum MidiFormat {
    MIDI_FORMAT_SINGLE_TRACK = 0,
    MIDI_FORMAT_MULTI_TRACK  = 1,
    MIDI_FORMAT_MULTI_SONG   = 2
} MidiFormat;

typedef enum MidiEventKind {
    MIDI_EVENT_CHANNEL,
    MIDI_EVENT_META,
    MIDI_EVENT_SYSEX,
} MidiEventKind;

typedef enum MidiChannelType {
    MIDI_NOTE_OFF       = 0x80,
    MIDI_NOTE_ON        = 0x90,
    MIDI_POLY_PRESSURE  = 0xA0,
    MIDI_CONTROL_CHANGE = 0xB0,
    MIDI_PROGRAM_CHANGE = 0xC0,
    MIDI_CHAN_PRESSURE  = 0xD0,
    MIDI_PITCH_BEND     = 0xE0,
} MidiChannelType;

enum {
    MIDI_META_SEQ_NUMBER   = 0x00,
    MIDI_META_TEXT         = 0x01,
    MIDI_META_COPYRIGHT    = 0x02,
    MIDI_META_TRACK_NAME   = 0x03,
    MIDI_META_INSTRUMENT   = 0x04,
    MIDI_META_LYRIC        = 0x05,
    MIDI_META_MARKER       = 0x06,
    MIDI_META_CHANNEL_PFX  = 0x20,
    MIDI_META_END_OF_TRACK = 0x2F,
    MIDI_META_TEMPO        = 0x51,
    MIDI_META_SMPTE_OFFSET = 0x54,
    MIDI_META_TIME_SIG     = 0x58,
    MIDI_META_KEY_SIG      = 0x59
};

typedef struct MidiEvent {
    uint32_t       delta;
    uint32_t       tick;
    MidiEventKind  kind;
    union {
        struct {
            MidiChannelType kind;
            union {
                struct {
                    uint8_t note;
                    uint8_t channel;
                    uint8_t velocity;
                } note_on;
                struct {
                    uint8_t note;
                    uint8_t channel;
                    uint8_t velocity;
                } note_off;
            } as;
        } channel;
        struct {
            uint8_t kind;
            union {
                struct {
                    uint32_t us_per_quarter;
                } tempo;
                struct {
                    uint8_t numerator;
                    uint8_t denominator;
                    uint8_t clocks_per_click;
                    uint8_t notated_32nds;
                } time_signature;
                struct {
                    int8_t  sharps_flats;
                    uint8_t is_minor;
                } key_signature;
            } as;
        } meta;
    } as;
} MidiEvent;

typedef struct MidiTrack {
    MidiEvent* items;
    size_t     count;
    size_t     capacity;
} MidiTrack;

typedef struct Midi {
    MidiFormat format;
    uint16_t   ntracks;
    uint16_t   division;
    MidiTrack* tracks;
} Midi;

#if defined(__cplusplus)
extern "C" {  
#endif

bool midi_parse(Midi* midi, const uint8_t* buffer, size_t size);
void midi_free(Midi* midi);

#if defined(RMIDI_IMPLEMENTATION)
static uint64_t read_variable_length(const uint8_t* base, size_t* offset, size_t limit)
{
    uint64_t value = 0;
    uint8_t  b     = 0;
    int      count = 0;
    do {
        if (*offset >= limit) break;
        b = base[*offset];
        *offset += 1;
        value = (value << 7) | (uint64_t)(b & 0b01111111);
    } while ((b & 0b10000000) && ++count < 4);
    return value;
}

void midi_free(Midi* midi)
{
    for (uint16_t i = 0; i < midi->ntracks; i++) {
        da_free(&midi->tracks[i]);
    }
    RMIDI_FREE(midi->tracks);
    midi->tracks  = NULL;
    midi->ntracks = 0;
}

bool midi_parse(Midi* midi, const uint8_t* buffer, size_t size)
{
    static const char* MIDI_HEADER = "MThd";
    static const char* MIDI_TRACK  = "MTrk";
    static const int   SMPTE       = 0x8000;

    printf("READ\n");
    if (size < sizeof(RawMidiHeader)) return false;
    RawChunkHeader* chunk = (RawChunkHeader*)buffer;
    if (memcmp(chunk->tag, MIDI_HEADER, strlen(MIDI_HEADER)) != 0) return false;
    RawMidiHeader* midi_header = (RawMidiHeader*)chunk;
    size_t   hlen    = be32(midi_header->header.size);
    int      format  = be16(midi_header->format);
    int      div     = be16(midi_header->div);
    uint16_t ntracks = be16(midi_header->ntracks);
    if (div == 0 || (div & SMPTE)) return false; // WTF is SMPTE
    if (format > MIDI_FORMAT_MULTI_SONG || ntracks == 0) return false; // NAH

    midi->format   = (MidiFormat)format;
    midi->division = (uint16_t)div;
    midi->ntracks  = 0;
    midi->tracks   = (MidiTrack*)RMIDI_MALLOC(sizeof(MidiTrack) * ntracks);
    assert(midi->tracks != NULL);
    memset(midi->tracks, 0, sizeof(MidiTrack) * ntracks);

    size_t   offset = sizeof(RawChunkHeader) + hlen;
    uint16_t tracks = 0;
    while (offset + sizeof(RawChunkHeader) <= size && tracks < ntracks) {
        RawChunkHeader* header = (RawChunkHeader*)(buffer + offset);
        uint32_t trck_size   = be32(header->size);
        if (offset + sizeof(RawChunkHeader) + trck_size > size)
            trck_size = (uint32_t)(size - offset - sizeof(RawChunkHeader));

        if (memcmp(header->tag, MIDI_TRACK, sizeof(header->tag)) == 0) {
            MidiTrack* track       = &midi->tracks[tracks++];
            uint32_t   tick        = 0;
            int        running     = 0;
            size_t     trck_offset = sizeof(RawChunkHeader);
            size_t     trck_end    = sizeof(RawChunkHeader) + trck_size;
            while (trck_offset < trck_end) {
                uint32_t delta  = (uint32_t)read_variable_length((uint8_t*)header, &trck_offset, trck_end);
                tick           += delta;
                if (trck_offset >= trck_end) break;
                int status      = ((uint8_t*)header)[trck_offset++];
                if (status < 0x80) {
                    if (!running) break;
                    trck_offset--;
                    status = running;
                }

                // reset
                if (status == 0xFF) {
                    if (trck_offset >= trck_end) break;
                    uint8_t  meta = ((uint8_t*)header)[trck_offset++];
                    uint64_t len  = read_variable_length((uint8_t*)header, &trck_offset, trck_end);
                    if (len > trck_end - trck_offset) break;
                    const uint8_t* data = (uint8_t*)header + trck_offset;
                    trck_offset += len;
                    running      = 0;

                    MidiEvent event = {0};
                    event.delta     = delta;
                    event.tick      = tick;
                    event.kind      = MIDI_EVENT_META;
                    event.as.meta.kind = meta;
                    if (meta == MIDI_META_END_OF_TRACK) break;
                    if (meta == MIDI_META_TEMPO && len == 3) {
                        event.as.meta.as.tempo.us_per_quarter =
                            ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];
                        da_append(track, event);
                    }
                    if (meta == MIDI_META_TIME_SIG && len >= 4) {
                        event.as.meta.as.time_signature.numerator        = data[0];
                        event.as.meta.as.time_signature.denominator      = (uint8_t)(1u << (data[1] & 0x07));
                        event.as.meta.as.time_signature.clocks_per_click = data[2];
                        event.as.meta.as.time_signature.notated_32nds    = data[3];
                        da_append(track, event);
                    }
                    if (meta == MIDI_META_KEY_SIG && len == 2) {
                        event.as.meta.as.key_signature.sharps_flats = (int8_t)data[0];
                        event.as.meta.as.key_signature.is_minor     = data[1] ? 1 : 0;
                        da_append(track, event);
                    }
                    continue;
                }
                // sysex
                if (status == 0xF0 || status == 0xF7) {
                    uint64_t len = read_variable_length((uint8_t*)header, &trck_offset, trck_end);
                    if (len > trck_end - trck_offset) break;
                    trck_offset += len;
                    running      = 0;
                    continue;
                }
                if (status > 0xEF) break;

                running      = status;
                uint8_t ch   = status & 0x0F;
                uint8_t type = status & 0xF0;

                // skipped, I do not care
                size_t nbytes = (type == MIDI_PROGRAM_CHANGE || type == MIDI_CHAN_PRESSURE) ? 1 : 2;
                if (trck_end - trck_offset < nbytes) break;
                if (nbytes == 1) {
                    trck_offset += 1;
                    continue;
                }

                uint8_t key = ((uint8_t*)header)[trck_offset++];
                uint8_t vel = ((uint8_t*)header)[trck_offset++];
                if (type == MIDI_NOTE_ON && vel > 0) {
                    MidiEvent event = {0};
                    event.delta     = delta;
                    event.tick      = tick;
                    event.kind      = MIDI_EVENT_CHANNEL;
                    event.as.channel.kind = MIDI_NOTE_ON;
                    event.as.channel.as.note_on.note     = key;
                    event.as.channel.as.note_on.channel  = ch;
                    event.as.channel.as.note_on.velocity = vel;
                    da_append(track, event);
                } else if (type == MIDI_NOTE_ON || type == MIDI_NOTE_OFF) {
                    MidiEvent event = {0};
                    event.delta     = delta;
                    event.tick      = tick;
                    event.kind      = MIDI_EVENT_CHANNEL;
                    event.as.channel.kind = MIDI_NOTE_OFF;
                    event.as.channel.as.note_off.note     = key;
                    event.as.channel.as.note_off.channel  = ch;
                    event.as.channel.as.note_off.velocity = vel;
                    da_append(track, event);
                }
            }
        }
        offset += sizeof(RawChunkHeader) + trck_size;
    }
    midi->ntracks = tracks;
    return true;
}
#endif // RMIDI_IMPLEMENTATION

#if defined(__cplusplus)
}
#endif

#endif // RMIDI_H
