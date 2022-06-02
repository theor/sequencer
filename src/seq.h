#ifndef SEQ_H
#define SEQ_H

#include "config.h"
#include "pattern.h"
#include "Arduino.h"

typedef void (*MIDINoteCallback)(byte channel, byte note, byte velocity);
struct Seq
{

    size_t patternIndex;
    volatile byte _position;
    Pattern patterns[8] = {};

    byte _noteRunning;

    MIDINoteCallback _noteOn;
    MIDINoteCallback _noteOff;
    bool posChanged = false;

    void setNoteCallbacks(MIDINoteCallback on, MIDINoteCallback off)
    {
        _noteOn = on;
        _noteOff = off;
    }

    void toggleStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        Step& s = (patterns[patternIndex].pages[pageIndex].steps[stepPageIndex]);
        s.note = s.note != 0 ? 0 : 40;

    }

    Step getStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        return patterns[patternIndex].pages[pageIndex].steps[stepPageIndex];
    }
    bool hasStep(byte stepIndex) {
        size_t pageIndex = stepIndex / STEP_COUNT;
        size_t stepPageIndex = stepIndex % STEP_COUNT;
        return patterns[patternIndex].pages[pageIndex].steps[stepPageIndex].note != 0;
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
        _position = pos % STEP_COUNT;
        posChanged = true;

    }

    void triggerNotes()
    {
        noInterrupts();
        bool changed = this->posChanged;
        posChanged = false;
        auto position = _position;
        interrupts();
        if (!changed)
            return;
        Page page = patterns[patternIndex].pages[0];
        Step step = page.steps[position];

        if(step.note != 0) {
            if(_noteRunning != 0) {
                _noteOff(MIDI_CHANNEL, _noteRunning, 0);
                _noteRunning = 0;
            }
            _noteOn(MIDI_CHANNEL, step.note, 127);
            _noteRunning = step.note;
        }
    }
};
static Seq seq;

#endif