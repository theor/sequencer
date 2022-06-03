#ifndef SEQ_H
#define SEQ_H

#include "config.h"
#include "pattern.h"
#include "Arduino.h"

typedef void (*MIDINoteCallback)(byte channel, byte note, byte velocity);

struct Track {
    static const int PATTERN_COUNT = 8;
    byte midiChannel;
    Pattern patterns[PATTERN_COUNT] = {};
    size_t patternIndex = 0;
    size_t queuedPatternIndex = 0;

    byte _noteRunning = 0;

    void setPattern(byte patternIndex, bool queue) {
        byte clamped = max(0, min(patternIndex, PATTERN_COUNT));
        if (queue)
            queuedPatternIndex = clamped;
        else {
            // same index means no queuing back later
            queuedPatternIndex = clamped;
            this->patternIndex = clamped;
        }
    }

    Pattern &getPattern() {
        return patterns[patternIndex];
    }

    bool hasQueuePattern() {
        return queuedPatternIndex != patternIndex;
    }

    // Steps

    void toggleStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        Pattern &pattern = getPattern();
        Step &s = (pattern.pages[pageIndex].steps[stepPageIndex]);
        s.active = !s.active;
        if (s.active) {
            s.note = pattern.note;
            s.velocity = 127;
        }

    }

    Step &getStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        return patterns[patternIndex].pages[pageIndex].steps[stepPageIndex];
    }

    bool hasStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        return patterns[patternIndex].pages[pageIndex].steps[stepPageIndex].active;
    }

    void setStep(byte stepIndex, Step step) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        patterns[patternIndex].pages[pageIndex].steps[stepPageIndex] = step;
    }
};

// bass snare chh crash
// 24 26 30 37

struct Seq {
    static const size_t TRACK_COUNT = 8;
    Track tracks[TRACK_COUNT] = {
            {1},
            {2},
            {3},
            {4},
            {5, {{36}}},
             {5, {{38}}},
             {5, {{42}}},
             {5, {{49}}},
            };

            byte currentTrackIndex = 0;

            void setCurrentTrack(byte trackIndex) { currentTrackIndex = trackIndex; }
            Track& getCurrentTrack() { return tracks[currentTrackIndex]; }


            volatile byte _position;

            MIDINoteCallback _noteOn;
            MIDINoteCallback _noteOff;
            bool posChanged = false;
            byte position = 0;

            void setNoteCallbacks(MIDINoteCallback on, MIDINoteCallback off) {
                _noteOn = on;
                _noteOff = off;
            }

            // Patterns



            void step(uint8_t pos) {
                // increment the position
                _position = pos;
                posChanged = true;

            }

            void triggerNotes() {
                noInterrupts();
                bool changed = this->posChanged;
                posChanged = false;
                auto rawPosition = _position;
                interrupts();
                if (!changed)
                    return;

                position = rawPosition % STEP_COUNT;
                for (size_t i = 0; i < TRACK_COUNT; ++i) {
                    Track &track = tracks[i];
                    // TODO pages
                    if (position == 0 &&
                        rawPosition / STEP_COUNT > 0)  // not the first pattern, might have a queued pattern
                    {
                        if (track.queuedPatternIndex != track.patternIndex)
                            track.patternIndex = track.queuedPatternIndex;
                    }
                    Page page = track.patterns[track.patternIndex].pages[0];
                    Step step = page.steps[position];

                    if (track._noteRunning != 0) {
                        _noteOff(track.midiChannel, track._noteRunning, 0);
                        track._noteRunning = 0;
                    }
                    if (step.isActive()) {
                        _noteOn(track.midiChannel, step.note, step.velocity);
                        track._noteRunning = step.note;
                    }
                }
            }
    };

    static Seq seq;

#endif