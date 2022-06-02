#ifndef PATTERN_H
#define PATTERN_H
#include "config.h"

struct Step
{
    uint8_t note;

    void dump()
    {
        Serial.print(note);
        Serial.print(" ");
    }
};
Step makeStep(byte note) {
    Step s;
    s.note = note;
    return s;
}

struct Page
{
    Step steps[STEP_COUNT] = {
        {42},
        {40},
        {40},
        {40},
        {40},
        {40},
        {40},
        {40},
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