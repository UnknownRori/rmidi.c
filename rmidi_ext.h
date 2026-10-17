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

#ifndef RMIDI_EXT_H
#define RMIDI_EXT_H

#include "rmidi.h"

typedef struct MidiTempo {
    uint32_t tick;
    uint32_t us_per_quarter;
    double   seconds;
    size_t   order;
} MidiTempo;
 
da_decl(MidiTempo, MidiTempoMap)

typedef struct MidiNote {
    double  start;
    double  end;
    uint8_t key;
    uint8_t channel;
    uint8_t velocity;
} MidiNote;

da_decl(MidiNote, MidiNoteList);

typedef struct MidiTimeSig {
    uint32_t tick;
    uint8_t  numerator;
    uint8_t  denominator;
    uint32_t bar_at;
    size_t   order;
} MidiTimeSig;

da_decl(MidiTimeSig, MidiTimeSigMap);

typedef struct MidiPosition {
    uint32_t bar;
    uint32_t beat;
    double   beat_fraction;
    uint8_t  numerator;
    uint8_t  denominator;
} MidiPosition;

double midi_tick_to_seconds   (const Midi* midi, const           MidiTempoMap*   map, uint32_t      tick);
double midi_seconds_to_ticks  (const Midi* midi, const           MidiTempoMap*   map, double        seconds);
void   midi_build_tempo_map   (const Midi* midi, MidiTempoMap*   map);
void   midi_collect_notes     (const Midi* midi, const           MidiTempoMap*   map, MidiNoteList* notes);
void   midi_build_timesig_map (const Midi* midi, MidiTimeSigMap* map);
void   midi_ticks_to_position (const Midi* midi, const           MidiTimeSigMap* map, MidiPosition* pos, double tick);

#if defined(RMIDI_EXT_IMPLEMENTATION)
static int midi_tempo_cmp(const void* a, const void* b)
{
    const MidiTempo* x = (const MidiTempo*)a;
    const MidiTempo* y = (const MidiTempo*)b;
    if (x->tick != y->tick) return x->tick < y->tick ? -1 : 1;
    return x->order < y->order ? -1 : (x->order > y->order);
}
 
void midi_build_tempo_map(const Midi* midi, MidiTempoMap* map)
{
    da_reset(map);
    MidiTempo def = { 0, 500000, 0.0, 0 };
    da_append(map, def);
    size_t order = 1;
    for (uint16_t i = 0; i < midi->ntracks; i++) {
        da_foreach(MidiEvent, ev, &midi->tracks[i]) {
            if (ev->kind != MIDI_EVENT_META || ev->as.meta.kind != MIDI_META_TEMPO) continue;
            MidiTempo t = { ev->tick, ev->as.meta.as.tempo.us_per_quarter, 0.0, order++ };
            da_append(map, t);
        }
    }
    if (map->count > 1) qsort(map->items, map->count, sizeof(MidiTempo), midi_tempo_cmp);
 
    size_t n = 0;
    for (size_t i = 0; i < map->count; i++) {
        if (n > 0 && map->items[n - 1].tick == map->items[i].tick) 
            map->items[n - 1] = map->items[i];
        else
            map->items[n++]     = map->items[i];
    }
    map->count = n;
 
    map->items[0].seconds = 0.0;
    for (size_t i = 1; i < map->count; i++) {
        const MidiTempo* p = &map->items[i - 1];
        map->items[i].seconds = p->seconds +
            (double)(map->items[i].tick - p->tick) * p->us_per_quarter / midi->division / 1e6;
    }
}
 
double midi_tick_to_seconds(const Midi* midi, const MidiTempoMap* map, uint32_t tick)
{
    assert(map->count > 0);
    size_t lo = 0, hi = map->count;
    while (hi - lo > 1) {
        size_t mid = (lo + hi) / 2;
        if (map->items[mid].tick <= tick) lo = mid;
        else                              hi = mid;
    }
    const MidiTempo* t = &map->items[lo];
    return t->seconds + (double)(tick - t->tick) * t->us_per_quarter / midi->division / 1e6;
}

static int midi_note_cmp(const void* a, const void* b)
{
    const MidiNote* x = (const MidiNote*)a;
    const MidiNote* y = (const MidiNote*)b;
    if (x->start != y->start) return x->start < y->start ? -1 : 1;
    return (int)x->key - (int)y->key;
}
 
void midi_collect_notes(const Midi* midi, const MidiTempoMap* map, MidiNoteList* notes)
{
    da_reset(notes);
    for (uint16_t i = 0; i < midi->ntracks; i++) {
        const MidiTrack* track = &midi->tracks[i];
 
        long     open[MAX_CHANNEL][128];
        uint32_t last_tick = 0;
        for (int c = 0; c < MAX_CHANNEL; c++)
            for (int k = 0; k < 128; k++) open[c][k] = -1;
 
        da_foreach(MidiEvent, ev, track) {
            last_tick = ev->tick;
            if (ev->kind != MIDI_EVENT_CHANNEL) continue;
 
            if (ev->as.channel.kind == MIDI_NOTE_ON) {
                uint8_t ch  = ev->as.channel.as.note_on.channel & 0x0F;
                uint8_t key = ev->as.channel.as.note_on.note & 0x7F;
                double  now = midi_tick_to_seconds(midi, map, ev->tick);
                if (open[ch][key] >= 0) notes->items[open[ch][key]].end = now;
 
                MidiNote n = { now, now, key, ch, ev->as.channel.as.note_on.velocity };
                open[ch][key] = (long)notes->count;
                da_append(notes, n);
            } else if (ev->as.channel.kind == MIDI_NOTE_OFF) {
                uint8_t ch  = ev->as.channel.as.note_off.channel & 0x0F;
                uint8_t key = ev->as.channel.as.note_off.note & 0x7F;
                if (open[ch][key] >= 0) {
                    notes->items[open[ch][key]].end = midi_tick_to_seconds(midi, map, ev->tick);
                    open[ch][key] = -1;
                }
            }
        }
 
        double track_end = midi_tick_to_seconds(midi, map, last_tick);
        for (int c = 0; c < MAX_CHANNEL; c++)
            for (int k = 0; k < 128; k++)
                if (open[c][k] >= 0) notes->items[open[c][k]].end = track_end;
    }
    if (notes->count > 1) qsort(notes->items, notes->count, sizeof(MidiNote), midi_note_cmp);
}

double midi_seconds_to_ticks(const Midi* midi, const MidiTempoMap* map, double seconds)
{
    assert(map->count > 0);
    if (seconds <= 0.0) return 0.0;
    size_t lo = 0, hi = map->count;
    while (hi - lo > 1) {
        size_t mid = (lo + hi) / 2;
        if (map->items[mid].seconds <= seconds) 
            lo = mid;
        else
            hi = mid;
    }
    const MidiTempo* t = &map->items[lo];
    return t->tick + (seconds - t->seconds) * 1e6 * midi->division / t->us_per_quarter;
}

static uint32_t midi_beat_ticks(const Midi* midi, const MidiTimeSig* ts)
{
    uint32_t t = (uint32_t)(midi->division * 4u / ts->denominator);
    return t ? t : 1;
}
static uint32_t midi_bar_ticks(const Midi* midi, const MidiTimeSig* ts)
{
    uint32_t t = ts->numerator * midi_beat_ticks(midi, ts);
    return t ? t : 1;
}
 
static int midi_timesig_cmp(const void* a, const void* b)
{
    const MidiTimeSig* x = (const MidiTimeSig*)a;
    const MidiTimeSig* y = (const MidiTimeSig*)b;
    if (x->tick != y->tick) return x->tick < y->tick ? -1 : 1;
    return x->order < y->order ? -1 : (x->order > y->order);
}
 
void midi_build_timesig_map(const Midi* midi, MidiTimeSigMap* map)
{
    da_reset(map);
    MidiTimeSig def = { 0, 4, 4, 0, 0 };
    da_append(map, def);
    size_t order = 1;
    for (uint16_t i = 0; i < midi->ntracks; i++) {
        da_foreach(MidiEvent, ev, &midi->tracks[i]) {
            if (ev->kind != MIDI_EVENT_META || ev->as.meta.kind != MIDI_META_TIME_SIG) continue;
            MidiTimeSig ts = { 
                ev->tick,
                ev->as.meta.as.time_signature.numerator,
                ev->as.meta.as.time_signature.denominator, 0, 
                order++,
            };
            if (ts.numerator == 0 || ts.denominator == 0) continue; // no way bwuh
            da_append(map, ts);
        }
    }
    if (map->count > 1) qsort(map->items, map->count, sizeof(MidiTimeSig), midi_timesig_cmp);
 
    size_t n = 0;
    for (size_t i = 0; i < map->count; i++) {
        if (n > 0 && map->items[n - 1].tick == map->items[i].tick) 
            map->items[n - 1] = map->items[i];
        else
            map->items[n++]     = map->items[i];
    }
    map->count = n;
 
    map->items[0].bar_at = 0;
    for (size_t i = 1; i < map->count; i++) {
        const MidiTimeSig* p = &map->items[i - 1];
        map->items[i].bar_at = p->bar_at + (map->items[i].tick - p->tick) / midi_bar_ticks(midi, p);
    }
}

void midi_ticks_to_position(const Midi* midi, const MidiTimeSigMap* map, MidiPosition* pos, double tick)
{
    assert(map->count > 0);
    assert(pos != NULL);
    if (tick < 0.0) tick = 0.0;
    size_t lo = 0, hi = map->count;
    while (hi - lo > 1) {
        size_t mid = (lo + hi) / 2;
        if (map->items[mid].tick <= tick) 
            lo = mid;
        else
            hi = mid;
    }
    const MidiTimeSig* ts = &map->items[lo];
    double   bar_ticks  = midi_bar_ticks(midi, ts);
    double   beat_ticks = midi_beat_ticks(midi, ts);
    double   since      = tick - ts->tick;
    uint32_t bars       = (uint32_t)(since / bar_ticks);
    double   in_bar     = since - bars * bar_ticks;
    uint32_t beats      = (uint32_t)(in_bar / beat_ticks);
 
    pos->bar           = ts->bar_at + bars + 1;
    pos->beat          = beats + 1;
    pos->beat_fraction = (in_bar - beats * beat_ticks) / beat_ticks;
    pos->numerator     = ts->numerator;
    pos->denominator   = ts->denominator;
}
#endif // RMIDI_EXT_IMPLEMENTATION
#endif // RMIDI_EXT_H
