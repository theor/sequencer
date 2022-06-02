#ifndef PATTERN_H
#define PATTERN_H

#include <cstdio>
#include <usb_serial.h>
#include "config.h"

struct Step
{
    uint8_t active;
    uint8_t note;
    uint8_t velocity;

    bool isActive() const { return active; }

    void dump()
    {
        Serial.print(note);
        Serial.print(" v");
        Serial.print(velocity);
        Serial.print(" a");
        Serial.print(active);
    }
};
Step makeStep(uint8_t note, uint8_t velocity = 127) {
    Step s;
    s.note = note;
    s.velocity = velocity;
    s.active = 1;
    return s;
}

struct Page
{
    Step steps[STEP_COUNT] = {
        {42, 127},
        {40, 100},
        {40, 64},
        {40, 96},
        {40, 32},
        {40, 48},
        {40, 127},
        {40, 96},
    };

    void dump()
    {
        for (size_t i = 0; i < STEP_COUNT; i++)
        {
            steps[i].dump();
        }
        Serial.println();
    }
};

struct Pattern
{
    uint8_t note = 48;
    Page pages[1] = {
        {},
    };

    void dump()
    {

        for (size_t i = 0; i < PAGE_COUNT; i++)
        {
            Serial.print("Page ");
            Serial.println(i);
            pages[i].dump();
        }
    }
};
#endif