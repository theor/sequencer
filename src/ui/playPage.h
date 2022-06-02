//
// Created by theor on 2022-06-02.
//

#ifndef INC_220531_123645_TEENSY41_PLAYPAGE_H
#define INC_220531_123645_TEENSY41_PLAYPAGE_H
#include <Arduino.h>
#include "../config.h"
#include "../seq.h"
#include "../display.h"
#include "inputs.h"
void drawSteps() {

    for (size_t i = 0; i < STEP_COUNT; i++) {
        const int sqSize = 4;
        const int sqSizeHalf = sqSize / 2;
        const int leftMargin = sqSizeHalf;
        const int sqSizeMarginRight = sqSize + 2;
        if (seq.hasStep(i)) {
            int x = leftMargin + i * sqSizeMarginRight;
            int y = SCREEN_HEIGHT - sqSizeMarginRight;
            if (i % 4 == 0)
                display.drawRect(x, y, sqSize, sqSize, COLOR_WHITE);
            else
                display.fillRect(x, y, sqSize, sqSize, COLOR_WHITE);
        } else {
            display.drawPixel(leftMargin + i * sqSizeMarginRight + sqSizeHalf, SCREEN_HEIGHT - sqSizeHalf,
                              COLOR_WHITE);
        }
    }
}

static const char* noteNames[] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
char noteBuffer[4];

void prepareNoteName(byte midiNote) {
    sprintf(noteBuffer, "%s%i", noteNames[midiNote % 12], ((int)midiNote / 12)-1);
}
void printNote(byte midiNote) {
    display.setCursor(20, 9);
    display.setTextSize(2);
    prepareNoteName(midiNote);
    display.print(noteBuffer);

}
void drawPlayPage() {
    if(Inputs::instance.heldStep != -1){
        const Step &step = seq.getStep(Inputs::instance.heldStep);
        if(step.isActive()) {
            display.setTextSize(1);
            display.setCursor(2, 2);
            display.print(Inputs::instance.heldStep);
            printNote(step.note);
        }
    } else {
        printNote(seq.getPattern().note);
    }

    drawSteps();
}

#endif //INC_220531_123645_TEENSY41_PLAYPAGE_H
