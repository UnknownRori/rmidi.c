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
#include <math.h>

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

typedef struct MidiBar {
    uint32_t start_tick;
    uint32_t length_ticks;
    uint32_t beat_ticks;
    uint8_t  beats;
} MidiBar;

#define MIDI_TIME_END 1.0e18

typedef enum MidiChordQuality {
    MIDI_CHORD_NONE,
    MIDI_CHORD_POWER,
    MIDI_CHORD_MAJOR,
    MIDI_CHORD_MINOR,
    MIDI_CHORD_DIM,
    MIDI_CHORD_AUG,
    MIDI_CHORD_SUS2,
    MIDI_CHORD_SUS4,
    MIDI_CHORD_DOM7,
    MIDI_CHORD_MAJ7,
    MIDI_CHORD_MIN7,
    MIDI_CHORD_DIM7,
    MIDI_CHORD_HALF_DIM7,
} MidiChordQuality;

typedef struct MidiChord {
    MidiChordQuality quality;
    uint8_t          root;
    uint8_t          bass;
    uint8_t          notes;
} MidiChord;

typedef struct MidiChordTone {
    char        name[4];
    const char* degree;
} MidiChordTone;

typedef struct MidiKey {
    uint8_t tonic;
    uint8_t is_minor;
    int8_t  sharps_flats;
    double  confidence;
} MidiKey;

typedef struct MidiKeySig {
    uint32_t tick;
    int8_t   sharps_flats;
    uint8_t  is_minor;
    size_t   order;
} MidiKeySig;

da_decl(MidiKeySig, MidiKeySigMap)

double midi_tick_to_seconds       (const Midi* midi, const           MidiTempoMap*   map, uint32_t      tick);
double midi_seconds_to_ticks      (const Midi* midi, const           MidiTempoMap*   map, double        seconds);
void   midi_build_tempo_map       (const Midi* midi, MidiTempoMap*   map);
void   midi_collect_notes         (const Midi* midi, const           MidiTempoMap*   map, MidiNoteList* notes);
void   midi_build_timesig_map     (const Midi* midi, MidiTimeSigMap* map);
void   midi_ticks_to_position     (const Midi* midi, const           MidiTimeSigMap* map, MidiPosition* pos, double tick);
void   midi_bar_at                (const Midi* midi, const           MidiTimeSigMap* map, MidiBar*      bar, double tick);
const MidiTimeSig* midi_timesig_at(const MidiTimeSigMap* map, double tick);
const MidiTempo*   midi_tempo_at          (const MidiTempoMap* map, double tick);
double             midi_bpm_at            (const MidiTempoMap* map, double tick);
bool               midi_detect_chord      (const MidiNoteList* notes, MidiChord* chord, int channel, double from, double to);
bool               midi_detect_key        (const MidiNoteList* notes, MidiKey* key, int channel, double from, double to);
const char*        midi_pitch_class_name  (uint8_t pc, bool flats);
void               midi_chord_name        (const MidiChord* chord, bool flats, char* out, size_t cap);
void               midi_key_name          (const MidiKey* key, char* out, size_t cap);
void               midi_chord_roman       (const MidiChord* chord, const MidiKey* key, char* out, size_t cap);
void               midi_build_keysig_map  (const Midi* midi, MidiKeySigMap* map);
bool               midi_keysig_at         (const MidiKeySigMap* map, MidiKey* key, double tick);
int                midi_chord_tones       (const MidiChord* chord, bool flats, MidiChordTone* out, int cap);

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

const MidiTimeSig* midi_timesig_at(const MidiTimeSigMap* map, double tick)
{
    assert(map->count > 0);
    size_t lo = 0, hi = map->count;
    while (hi - lo > 1) {
        size_t mid = (lo + hi) / 2;
        if (map->items[mid].tick <= tick) lo = mid;
        else                              hi = mid;
    }
    return &map->items[lo];
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

void midi_bar_at(const Midi* midi, const MidiTimeSigMap* map, MidiBar* bar, double tick)
{
    const MidiTimeSig* ts     = midi_timesig_at(map, tick);
    if (tick < ts->tick) tick = ts->tick;
    uint32_t bar_ticks        = midi_bar_ticks(midi, ts);
    uint32_t bars             = (uint32_t)((tick - ts->tick) / bar_ticks);
 
    bar->start_tick   = ts->tick + bars * bar_ticks;
    bar->length_ticks = bar_ticks;
    bar->beat_ticks   = midi_beat_ticks(midi, ts);
    bar->beats        = ts->numerator;
}

const MidiTempo* midi_tempo_at(const MidiTempoMap* map, double tick)
{
    assert(map->count > 0);
    size_t lo = 0, hi = map->count;
    while (hi - lo > 1) {
        size_t mid = (lo + hi) / 2;
        if (map->items[mid].tick <= tick) lo = mid;
        else                              hi = mid;
    }
    return &map->items[lo];
}

double midi_bpm_at(const MidiTempoMap* map, double tick)
{
    const MidiTempo* t = midi_tempo_at(map, tick);
    return t->us_per_quarter ? 60000000.0 / t->us_per_quarter : 0.0;
}

static int midi_popcount(uint16_t v)
{
    int n = 0;
    for (; v; v &= (uint16_t)(v - 1)) n++;
    return n;
}

static const struct {
    MidiChordQuality quality;
    uint16_t         mask;
} MIDI_CHORD_TEMPLATES[] = {
    { MIDI_CHORD_POWER,     0x081 },
    { MIDI_CHORD_MAJOR,     0x091 },
    { MIDI_CHORD_MINOR,     0x089 },
    { MIDI_CHORD_DIM,       0x049 },
    { MIDI_CHORD_AUG,       0x111 },
    { MIDI_CHORD_SUS2,      0x085 },
    { MIDI_CHORD_SUS4,      0x0A1 },
    { MIDI_CHORD_DOM7,      0x491 },
    { MIDI_CHORD_MAJ7,      0x891 },
    { MIDI_CHORD_MIN7,      0x489 },
    { MIDI_CHORD_DIM7,      0x249 },
    { MIDI_CHORD_HALF_DIM7, 0x449 },
};

static const char* const MIDI_CHORD_SUFFIX[] = {
    [MIDI_CHORD_NONE]      = "",
    [MIDI_CHORD_POWER]     = "5",
    [MIDI_CHORD_MAJOR]     = "",
    [MIDI_CHORD_MINOR]     = "m",
    [MIDI_CHORD_DIM]       = "dim",
    [MIDI_CHORD_AUG]       = "aug",
    [MIDI_CHORD_SUS2]      = "sus2",
    [MIDI_CHORD_SUS4]      = "sus4",
    [MIDI_CHORD_DOM7]      = "7",
    [MIDI_CHORD_MAJ7]      = "maj7",
    [MIDI_CHORD_MIN7]      = "m7",
    [MIDI_CHORD_DIM7]      = "dim7",
    [MIDI_CHORD_HALF_DIM7] = "m7b5",
};

bool midi_detect_chord(const MidiNoteList* notes, MidiChord* chord, int channel, double from, double to)
{
    uint16_t mask = 0;
    int      bass = 128;
    for (size_t i = 0; i < notes->count; i++) {
        const MidiNote* n = &notes->items[i];
        if (n->start > to) break;
        if (n->end <= from) continue;
        if (channel < 0 ? n->channel == 9 : n->channel != channel) continue;
        mask |= (uint16_t)(1u << (n->key % 12));
        if (n->key < bass) bass = n->key;
    }

    memset(chord, 0, sizeof(*chord));
    if (midi_popcount(mask) < 2) return false;
    chord->bass  = (uint8_t)(bass % 12);
    chord->notes = (uint8_t)midi_popcount(mask);

    int best = -1000;
    for (int root = 0; root < 12; root++) {
        if (!(mask & (1u << root))) continue;
        uint16_t rotated = (uint16_t)(((mask >> root) | (mask << (12 - root))) & 0x0FFF);
        for (size_t t = 0; t < sizeof(MIDI_CHORD_TEMPLATES) / sizeof(MIDI_CHORD_TEMPLATES[0]); t++) {
            uint16_t tpl = MIDI_CHORD_TEMPLATES[t].mask;
            if (tpl & ~rotated) continue;
            int score = midi_popcount(tpl) * 4
                      - midi_popcount((uint16_t)(rotated & ~tpl)) * 3
                      + (root == chord->bass ? 2 : 0);
            if (score > best) {
                best           = score;
                chord->quality = MIDI_CHORD_TEMPLATES[t].quality;
                chord->root    = (uint8_t)root;
            }
        }
    }
    return chord->quality != MIDI_CHORD_NONE;
}

static double midi_correlate(const double* x, const double* y)
{
    double mx = 0.0, my = 0.0;
    for (int i = 0; i < 12; i++) { mx += x[i]; my += y[i]; }
    mx /= 12.0;
    my /= 12.0;

    double sxy = 0.0, sxx = 0.0, syy = 0.0;
    for (int i = 0; i < 12; i++) {
        double dx = x[i] - mx;
        double dy = y[i] - my;
        sxy += dx * dy;
        sxx += dx * dx;
        syy += dy * dy;
    }
    if (sxx <= 0.0 || syy <= 0.0) return -2.0;
    return sxy / sqrt(sxx * syy);
}

bool midi_detect_key(const MidiNoteList* notes, MidiKey* key, int channel, double from, double to)
{
    static const double MAJOR[12] = { 6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88 };
    static const double MINOR[12] = { 6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17 };
    static const int8_t MAJOR_SIGNATURE[12] = { 0, -5, 2, -3, 4, -1, 6, 1, -4, 3, -2, 5 };

    double histogram[12] = {0};
    for (size_t i = 0; i < notes->count; i++) {
        const MidiNote* n = &notes->items[i];
        if (n->start >= to) break;
        if (n->end <= from) continue;
        if (channel < 0 ? n->channel == 9 : n->channel != channel) continue;
        double a = n->start > from ? n->start : from;
        double b = n->end   < to   ? n->end   : to;
        if (b > a) histogram[n->key % 12] += b - a;
    }

    memset(key, 0, sizeof(*key));
    double best  = -2.0;
    bool   found = false;
    for (int tonic = 0; tonic < 12; tonic++) {
        for (int minor = 0; minor < 2; minor++) {
            const double* profile = minor ? MINOR : MAJOR;
            double rotated[12];
            for (int pc = 0; pc < 12; pc++) rotated[pc] = profile[(pc - tonic + 12) % 12];
            double r = midi_correlate(histogram, rotated);
            if (r > best) {
                best          = r;
                key->tonic    = (uint8_t)tonic;
                key->is_minor = (uint8_t)minor;
                found         = true;
            }
        }
    }
    if (!found) return false;
    key->confidence   = best;
    key->sharps_flats = MAJOR_SIGNATURE[(key->tonic + (key->is_minor ? 3 : 0)) % 12];
    return true;
}

const char* midi_pitch_class_name(uint8_t pc, bool flats)
{
    static const char* const SHARP[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    static const char* const FLAT[12]  = { "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };
    return (flats ? FLAT : SHARP)[pc % 12];
}

void midi_chord_name(const MidiChord* chord, bool flats, char* out, size_t cap)
{
    if (chord->quality == MIDI_CHORD_NONE) {
        snprintf(out, cap, "N.C.");
        return;
    }
    const char* root   = midi_pitch_class_name(chord->root, flats);
    const char* suffix = MIDI_CHORD_SUFFIX[chord->quality];
    if (chord->bass != chord->root)
        snprintf(out, cap, "%s%s/%s", root, suffix, midi_pitch_class_name(chord->bass, flats));
    else
        snprintf(out, cap, "%s%s", root, suffix);
}

void midi_key_name(const MidiKey* key, char* out, size_t cap)
{
    static const char* const KEY_NAMES[15][2] = {
        { "Cb", "Ab" },
        { "Gb", "Eb" },
        { "Db", "Bb" },
        { "Ab", "F"  },
        { "Eb", "C"  },
        { "Bb", "G"  },
        { "F",  "D"  },
        { "C",  "A"  },
        { "G",  "E"  },
        { "D",  "B"  },
        { "A",  "F#" },
        { "E",  "C#" },
        { "B",  "G#" },
        { "F#", "D#" },
        { "C#", "A#" } 
    };
    int idx = key->sharps_flats + 7;
    if (idx < 0) idx = 0;
    if (idx > 14) idx = 14;

    bool minor = key->is_minor; // Can be your forced true value

    snprintf(out, cap, "%s %s", KEY_NAMES[idx][minor ? 1 : 0], minor ? "min" : "maj");
}

void midi_chord_roman(const MidiChord* chord, const MidiKey* key, char* out, size_t cap)
{
    static const char* const MAJOR_NUMERAL[12] = { "I", "bII", "II", "bIII", "III", "IV", "#IV", "V", "bVI", "VI", "bVII", "VII" };
    static const char* const MINOR_NUMERAL[12] = { "I", "bII", "II", "III", "#III", "IV", "#IV", "V", "VI", "#VI", "VII", "#VII" };
    static const struct {
        bool        lower;
        const char* suffix;
    } STYLE[] = {
        [MIDI_CHORD_NONE]      = { false, ""     },
        [MIDI_CHORD_POWER]     = { false, "5"    },
        [MIDI_CHORD_MAJOR]     = { false, ""     },
        [MIDI_CHORD_MINOR]     = { true,  ""     },
        [MIDI_CHORD_DIM]       = { true,  "°"    },
        [MIDI_CHORD_AUG]       = { false, "+"    },
        [MIDI_CHORD_SUS2]      = { false, "sus2" },
        [MIDI_CHORD_SUS4]      = { false, "sus4" },
        [MIDI_CHORD_DOM7]      = { false, "7"    },
        [MIDI_CHORD_MAJ7]      = { false, "maj7" },
        [MIDI_CHORD_MIN7]      = { true,  "7"    },
        [MIDI_CHORD_DIM7]      = { true,  "°7"   },
        [MIDI_CHORD_HALF_DIM7] = { true,  "ø7"   },
    };

    if (chord->quality == MIDI_CHORD_NONE) {
        snprintf(out, cap, "-");
        return;
    }
    int  offset = (chord->root - key->tonic + 12) % 12;
    char numeral[8];
    snprintf(numeral, sizeof numeral, "%s", (key->is_minor ? MINOR_NUMERAL : MAJOR_NUMERAL)[offset]);
    if (STYLE[chord->quality].lower)
        for (char* c = numeral; *c; c++)
            if (*c >= 'A' && *c <= 'Z') *c = (char)(*c + ('a' - 'A'));
    snprintf(out, cap, "%s%s", numeral, STYLE[chord->quality].suffix);
}

static int midi_keysig_cmp(const void* a, const void* b)
{
    const MidiKeySig* x = (const MidiKeySig*)a;
    const MidiKeySig* y = (const MidiKeySig*)b;
    if (x->tick != y->tick) return x->tick < y->tick ? -1 : 1;
    return x->order < y->order ? -1 : (x->order > y->order);
}

void midi_build_keysig_map(const Midi* midi, MidiKeySigMap* map)
{
    da_reset(map);
    size_t order = 0;
    for (uint16_t i = 0; i < midi->ntracks; i++) {
        da_foreach(MidiEvent, ev, &midi->tracks[i]) {
            if (ev->kind != MIDI_EVENT_META || ev->as.meta.kind != MIDI_META_KEY_SIG) continue;
            int8_t sharps_flats = ev->as.meta.as.key_signature.sharps_flats;
            if (sharps_flats < -7 || sharps_flats > 7) continue;
            MidiKeySig ks = { ev->tick, sharps_flats, ev->as.meta.as.key_signature.is_minor, order++ };
            da_append(map, ks);
        }
    }
    if (map->count > 1) qsort(map->items, map->count, sizeof(MidiKeySig), midi_keysig_cmp);

    size_t n = 0;
    for (size_t i = 0; i < map->count; i++) {
        if (n > 0 && map->items[n - 1].tick == map->items[i].tick)
            map->items[n - 1] = map->items[i];
        else
            map->items[n++]   = map->items[i];
    }
    map->count = n;
}

bool midi_keysig_at(const MidiKeySigMap* map, MidiKey* key, double tick)
{
    memset(key, 0, sizeof(*key));
    if (map->count == 0) return false;
    size_t lo = 0, hi = map->count;
    while (hi - lo > 1) {
        size_t mid = (lo + hi) / 2;
        if (map->items[mid].tick <= tick) lo = mid;
        else                              hi = mid;
    }
    const MidiKeySig* ks = &map->items[lo];
    int major = ((ks->sharps_flats * 7) % 12 + 12) % 12;
    key->tonic        = (uint8_t)(ks->is_minor ? (major + 9) % 12 : major);
    key->is_minor     = ks->is_minor;
    key->sharps_flats = ks->sharps_flats;
    key->confidence   = 1.0;
    return true;
}

static const char* const MIDI_INTERVAL_DEGREE[12] = { "R", "b2", "2", "b3", "3", "4", "b5", "5", "#5", "bb7", "b7", "7" };
static const int         MIDI_INTERVAL_STEP[12]   = { 0, 1, 1, 2, 2, 3, 4, 4, 4, 6, 6, 6 };

int midi_chord_tones(const MidiChord* chord, bool flats, MidiChordTone* out, int cap)
{
    static const char LETTERS[7] = { 'C', 'D', 'E', 'F', 'G', 'A', 'B' };
    static const int  NATURAL[7] = { 0, 2, 4, 5, 7, 9, 11 };

    if (chord->quality == MIDI_CHORD_NONE) return 0;
    uint16_t mask = 0;
    for (size_t t = 0; t < sizeof(MIDI_CHORD_TEMPLATES) / sizeof(MIDI_CHORD_TEMPLATES[0]); t++)
        if (MIDI_CHORD_TEMPLATES[t].quality == chord->quality) mask = MIDI_CHORD_TEMPLATES[t].mask;
    if (mask == 0) return 0;

    const char* root_name   = midi_pitch_class_name(chord->root, flats);
    int         root_letter = 0;
    for (int i = 0; i < 7; i++)
        if (LETTERS[i] == root_name[0]) root_letter = i;

    int n = 0;
    for (int semitones = 0; semitones < 12 && n < cap; semitones++) {
        if (!(mask & (1u << semitones))) continue;
        int         letter = (root_letter + MIDI_INTERVAL_STEP[semitones]) % 7;
        int         pc     = (chord->root + semitones) % 12;
        int         diff   = (pc - NATURAL[letter] + 12) % 12;
        const char* accidental = diff == 0 ? "" : diff == 1 ? "#" : diff == 2 ? "##" : diff == 11 ? "b" : diff == 10 ? "bb" : NULL;
        if (accidental) snprintf(out[n].name, sizeof out[n].name, "%c%s", LETTERS[letter], accidental);
        else            snprintf(out[n].name, sizeof out[n].name, "%s", midi_pitch_class_name((uint8_t)pc, flats));
        out[n].degree = MIDI_INTERVAL_DEGREE[semitones];
        n++;
    }
    return n;
}
#endif // RMIDI_EXT_IMPLEMENTATION
#endif // RMIDI_EXT_H
