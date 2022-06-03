#ifndef SEQ_H
#define SEQ_H

#include "config.h"
#include "pattern.h"
#include "Arduino.h"

typedef void (*MIDINoteCallback)(byte channel, byte note, byte velocity);
struct Seq
{

    size_t patternIndex;
    size_t queuedPatternIndex;
    volatile byte _position;
    static const int PATTERN_COUNT = 8;
    Pattern patterns[PATTERN_COUNT] = {};

    byte _noteRunning;

    MIDINoteCallback _noteOn;
    MIDINoteCallback _noteOff;
    bool posChanged = false;
    byte position = 0;

    void setNoteCallbacks(MIDINoteCallback on, MIDINoteCallback off)
    {
        _noteOn = on;
        _noteOff = off;
    }

    // Patterns

    void setPattern(byte patternIndex, bool queue) {
        byte clamped = max(0, min(patternIndex, PATTERN_COUNT));
        if(queue)
            queuedPatternIndex = clamped;
        else
        {
            // same index means no queuing back later
            queuedPatternIndex = clamped;
            this->patternIndex = clamped;
        }
    }

    Pattern& getPattern() {
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
        Step& s = (pattern.pages[pageIndex].steps[stepPageIndex]);
        s.active = !s.active;
        if(s.active) {
            s.note = pattern.note;
            s.velocity = 127;
        }

    }

    Step& getStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        return patterns[patternIndex].pages[pageIndex].steps[stepPageIndex];
    }
    bool hasStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        return patterns[patternIndex].pages[pageIndex].steps[stepPageIndex].active;
    }

    void setStep(byte stepIndex, Step step)
    {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        patterns[patternIndex].pages[pageIndex].steps[stepPageIndex] = step;
    }

    void step(uint8_t pos)
    {
        // increment the position
        _position = pos;
        posChanged = true;

    }

    void triggerNotes()
    {
        noInterrupts();
        bool changed = this->posChanged;
        posChanged = false;
        auto rawPosition = _position;
        interrupts();
        if (!changed)
            return;

        position = rawPosition % STEP_COUNT;
        // TODO pages
        if(position == 0 && rawPosition / STEP_COUNT > 0 && queuedPatternIndex != patternIndex) // not the first pattern, might have a queued pattern
        {
            patternIndex = queuedPatternIndex;
        }
        Page page = patterns[patternIndex].pages[0];
        Step step = page.steps[position];

        if(step.active != 0) {
            if(_noteRunning != 0) {
                _noteOff(MIDI_CHANNEL, _noteRunning, 0);
                _noteRunning = 0;
            }
            _noteOn(MIDI_CHANNEL, step.note, step.velocity);
            _noteRunning = step.note;
        }
    }
};
static Seq seq;

#endif