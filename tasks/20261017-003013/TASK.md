# wrapper for parsed raw midi into more something that easy to work with

- STATUS: CLOSED
- PRIORITY: 100
- TAGS: dx

No idea how this works but at least I can map out the tempo mapping, and note mapping

it would be cool to have

```c
double midi_tick_to_seconds   (const Midi* midi, const           MidiTempoMap*   map, uint32_t      tick);
double midi_seconds_to_ticks  (const Midi* midi, const           MidiTempoMap*   map, double        seconds);
void   midi_build_tempo_map   (const Midi* midi, MidiTempoMap*   map);
void   midi_collect_notes     (const Midi* midi, const           MidiTempoMap*   map, MidiNoteList* notes);
void   midi_build_timesig_map (const Midi* midi, MidiTimeSigMap* map);
void   midi_ticks_to_position (const Midi* midi, const           MidiTimeSigMap* map, MidiPosition* pos, double tick);
```
