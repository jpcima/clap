#pragma once

#include "private/std.h"
#include "fixedpoint.h"
#include "id.h"

#ifdef __cplusplus
extern "C" {
#endif

// event header
// must be the first attribute of the event
typedef struct clap_event_header {
   uint32_t size;     // event size including this header, eg: sizeof (clap_event_note)
   uint32_t time;     // time at which the event happens
   uint16_t space_id; // event space, see clap_host_event_registry
   uint16_t type;     // event type
   uint32_t flags;    // see clap_event_flags
} clap_event_header_t;

// The clap core event space
static const CLAP_CONSTEXPR uint16_t CLAP_CORE_EVENT_SPACE_ID = 0;

enum clap_event_flags {
   // indicate a live momentary event
   CLAP_EVENT_IS_LIVE = 1 << 0,

   // indicate that the event should not be recorded.
   // For example this is useful when a parameter changes because of a MIDI CC,
   // because if the host records both the MIDI CC automation and the parameter
   // automation there will be a conflict.
   CLAP_EVENT_DONT_RECORD = 1 << 1,
};

// Some of the following events overlap, a note on can be expressed with:
// - CLAP_EVENT_NOTE_ON
// - CLAP_EVENT_MIDI
// - CLAP_EVENT_MIDI2
//
// The preferred way of sending a note event is to use CLAP_EVENT_NOTE_*.
//
// The same event must not be sent twice: it is forbidden to send a the same note on
// encoded with both CLAP_EVENT_NOTE_ON and CLAP_EVENT_MIDI.
//
// The plugins are encouraged to be able to handle note events encoded as raw midi or midi2,
// or implement clap_plugin_event_filter and reject raw midi and midi2 events.
enum {
   // NOTE_ON and NOTE_OFF represents a key pressed and key released event.
   //
   // NOTE_CHOKE is meant to choke the voice(s), like in a drum machine when a closed hihat
   // chokes an open hihat.
   //
   // NOTE_END is sent by the plugin to the host. The port, channel and key are those given
   // by the host in the NOTE_ON event. In other words, this event is matched against the
   // plugin's note input port. NOTE_END is only requiered if the plugin marked at least
   // one of its parameters as polyphonic.
   //
   // When using polyphonic modulations, the host has to allocate and release voices for its
   // polyphonic modulator. Yet only the plugin effectively knows when the host should terminate
   // a voice. NOTE_END solves that issue in a non-intrusive and cooperative way.
   //
   // CLAP assumes that the host will allocate a unique voice on NOTE_ON event for a given port,
   // channel and key. This voice will run until the plugin will instruct the host to terminate
   // it by sending a NOTE_END event.
   //
   // Consider the following sequence:
   // - process()
   //    Host->Plugin NoteOn(port:0, channel:0, key:16, time:t0)
   //    Host->Plugin NoteOn(port:0, channel:0, key:64, time:t0)
   //    Host->Plugin NoteOff(port:0, channel:0, key:16, t1)
   //    Host->Plugin NoteOff(port:0, channel:0, key:64, t1)
   //    # on t2, both notes did terminate
   //    Host->Plugin NoteOn(port:0, channel:0, key:64, t3)
   //    # Here the plugin finished to process all the frames and will tell the host
   //    # to terminate the voice on key 16 but not 64, because a note has been started at t3
   //    Plugin->Host NoteEnd(port:0, channel:0, key:16, time:ignored)
   //
   // Those four events use clap_event_note.
   CLAP_EVENT_NOTE_ON,
   CLAP_EVENT_NOTE_OFF,
   CLAP_EVENT_NOTE_CHOKE,
   CLAP_EVENT_NOTE_END,

   // Represents a note expression.
   // Uses clap_event_note_expression.
   CLAP_EVENT_NOTE_EXPRESSION,

   // PARAM_VALUE sets the parameter's value; uses clap_event_param_value.
   // PARAM_MOD sets the parameter's modulation amount; uses clap_event_param_mod.
   //
   // The value heard is: param_value + param_mod.
   //
   // In case of a concurrent global value/modulation versus a polyphonic one,
   // the voice should only use the polyphonic one and the polyphonic modulation
   // amount will already include the monophonic signal.
   CLAP_EVENT_PARAM_VALUE,
   CLAP_EVENT_PARAM_MOD,

   // uses clap_event_param_gesture
   // Indicates that a parameter gesture begun or ended.
   CLAP_EVENT_PARAM_GESTURE_BEGIN,
   CLAP_EVENT_PARAM_GESTURE_END,

   CLAP_EVENT_TRANSPORT,  // update the transport info; clap_event_transport
   CLAP_EVENT_MIDI,       // raw midi event; clap_event_midi
   CLAP_EVENT_MIDI_SYSEX, // raw midi sysex event; clap_event_midi_sysex
   CLAP_EVENT_MIDI2,      // raw midi 2 event; clap_event_midi2
};
typedef int32_t clap_event_type;

/**
 * Note on, off, end and choke events.
 * In the case of note choke or end events:
 * - the velocity is ignored.
 * - key and channel are used to match active notes, a value of -1 matches all.
 */
typedef struct clap_event_note {
   clap_event_header_t header;

   int16_t port_index;
   int16_t key;      // 0..127
   int16_t channel;  // 0..15
   double  velocity; // 0..1
} clap_event_note_t;

enum {
   // with 0 < x <= 4, plain = 20 * log(x)
   CLAP_NOTE_EXPRESSION_VOLUME,

   // pan, 0 left, 0.5 center, 1 right
   CLAP_NOTE_EXPRESSION_PAN,

   // relative tuning in semitone, from -120 to +120
   CLAP_NOTE_EXPRESSION_TUNING,

   // 0..1
   CLAP_NOTE_EXPRESSION_VIBRATO,
   CLAP_NOTE_EXPRESSION_EXPRESSION,
   CLAP_NOTE_EXPRESSION_BRIGHTNESS,
   CLAP_NOTE_EXPRESSION_PRESSURE,
};
typedef int32_t clap_note_expression;

typedef struct clap_event_note_expression {
   clap_event_header_t header;

   clap_note_expression expression_id;

   // target a specific port, key and channel, -1 for global
   int16_t port_index;
   int16_t key;
   int16_t channel;

   double value; // see expression for the range
} clap_event_note_expression_t;

typedef struct clap_event_param_value {
   clap_event_header_t header;

   // target parameter
   clap_id param_id; // @ref clap_param_info.id
   void   *cookie;   // @ref clap_param_info.cookie

   // target a specific port, key and channel, -1 for global
   int16_t port_index;
   int16_t key;
   int16_t channel;

   double value;
} clap_event_param_value_t;

typedef struct clap_event_param_mod {
   clap_event_header_t header;

   // target parameter
   clap_id param_id; // @ref clap_param_info.id
   void   *cookie;   // @ref clap_param_info.cookie

   // target a specific port, key and channel, -1 for global
   int16_t port_index;
   int16_t key;
   int16_t channel;

   double amount; // modulation amount
} clap_event_param_mod_t;

typedef struct clap_event_param_gesture {
   clap_event_header_t header;

   // target parameter
   clap_id param_id; // @ref clap_param_info.id
} clap_event_param_gesture_t;

enum clap_transport_flags {
   CLAP_TRANSPORT_HAS_TEMPO = 1 << 0,
   CLAP_TRANSPORT_HAS_BEATS_TIMELINE = 1 << 1,
   CLAP_TRANSPORT_HAS_SECONDS_TIMELINE = 1 << 2,
   CLAP_TRANSPORT_HAS_TIME_SIGNATURE = 1 << 3,
   CLAP_TRANSPORT_IS_PLAYING = 1 << 4,
   CLAP_TRANSPORT_IS_RECORDING = 1 << 5,
   CLAP_TRANSPORT_IS_LOOP_ACTIVE = 1 << 6,
   CLAP_TRANSPORT_IS_WITHIN_PRE_ROLL = 1 << 7,
};

typedef struct clap_event_transport {
   clap_event_header_t header;

   uint32_t flags; // see clap_transport_flags

   clap_beattime song_pos_beats;   // position in beats
   clap_sectime  song_pos_seconds; // position in seconds

   double tempo;     // in bpm
   double tempo_inc; // tempo increment for each samples and until the next
                     // time info event

   clap_beattime loop_start_beats;
   clap_beattime loop_end_beats;
   clap_sectime  loop_start_seconds;
   clap_sectime  loop_end_seconds;

   clap_beattime bar_start;  // start pos of the current bar
   int32_t       bar_number; // bar at song pos 0 has the number 0

   int16_t tsig_num;   // time signature numerator
   int16_t tsig_denom; // time signature denominator
} clap_event_transport_t;

typedef struct clap_event_midi {
   clap_event_header_t header;

   uint16_t port_index;
   uint8_t  data[3];
} clap_event_midi_t;

typedef struct clap_event_midi_sysex {
   clap_event_header_t header;

   uint16_t       port_index;
   const uint8_t *buffer; // midi buffer
   uint32_t       size;
} clap_event_midi_sysex_t;

// While it is possible to use a series of midi2 event to send a sysex,
// prefer clap_event_midi_sysex if possible for efficiency.
typedef struct clap_event_midi2 {
   clap_event_header_t header;

   uint16_t port_index;
   uint32_t data[4];
} clap_event_midi2_t;

// Input event list, events must be sorted by time.
typedef struct clap_input_events {
   void *ctx; // reserved pointer for the list

   uint32_t (*size)(const struct clap_input_events *list);

   // Don't free the return event, it belongs to the list
   const clap_event_header_t *(*get)(const struct clap_input_events *list, uint32_t index);
} clap_input_events_t;

// Output event list, events must be sorted by time.
typedef struct clap_output_events {
   void *ctx; // reserved pointer for the list

   // Pushes a copy of the event
   // returns false if the event could not be pushed to the queue (out of memory?)
   bool (*try_push)(const struct clap_output_events *list, const clap_event_header_t *event);
} clap_output_events_t;

#ifdef __cplusplus
}
#endif
