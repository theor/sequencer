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
    const int sqSize = 4;
    const int sqSizeHalf = sqSize / 2;
    const int margin = sqSizeHalf;
    const int sqSizeMarginRight = sqSize + margin;

    int y = SCREEN_HEIGHT - sqSizeMarginRight;
    for (size_t i = 0; i < STEP_COUNT; i++) {

        int x = margin + i * sqSizeMarginRight;
        if (seq.hasStep(i)) {
            if (i % 4 == 0)
                display.drawRect(x, y, sqSize, sqSize, COLOR_WHITE);
            else
                display.fillRect(x, y, sqSize, sqSize, COLOR_WHITE);
        } else {
            display.drawPixel(margin + i * sqSizeMarginRight + sqSizeHalf, SCREEN_HEIGHT - sqSizeHalf,
                              COLOR_WHITE);
        }
        if(i == seq.position)
            display.drawFastHLine(x, y - 2, sqSize, COLOR_WHITE);
    }
}

static const char* noteNames[] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
char noteBuffer[4];

void prepareNoteName(byte midiNote) {
    sprintf(noteBuffer, "%s%i", noteNames[midiNote % 12], ((int)midiNote / 12)-1);
}
void printNote(byte midiNote) {
    display.setCursor(128 - 38, 2);
    display.setTextSize(2);
    prepareNoteName(midiNote);
    display.print(noteBuffer);

}
void printVelocity(byte midiNote) {
    display.setCursor(128 - 38, 20);
    display.setTextSize(2);
    display.print(midiNote);

}
void drawPlayPage() {
    display.setCursor(2, 2);
    display.setTextSize(2);
    display.print('p');
    display.print(seq.patternIndex);
    display.setTextColor(COLOR_WHITE, COLOR_BLACK);
    if(seq.hasQueuePattern()) {
        display.print(' ');
        display.setTextColor(COLOR_BLACK, COLOR_WHITE);
    display.print(seq.queuedPatternIndex);
        display.setTextColor(COLOR_WHITE, COLOR_BLACK);
    }
    if(Inputs::instance.heldStep != -1){
        const Step &step = seq.getStep(Inputs::instance.heldStep);
        if(step.isActive()) {
            display.setTextSize(1);
            display.setCursor(2, 2);
            display.print(Inputs::instance.heldStep);
            printNote(step.note);
            printVelocity(step.velocity);
        }
    } else {
        printNote(seq.getPattern().note);
    }

    drawSteps();
}

#endif //INC_220531_123645_TEENSY41_PLAYPAGE_H
