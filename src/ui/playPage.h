//
// Created by theor on 2022-06-02.
//

#ifndef INC_220531_123645_TEENSY41_PLAYPAGE_H
#define INC_220531_123645_TEENSY41_PLAYPAGE_H
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

void drawPlayPage() {
    if(Inputs::instance.heldStep != -1){
        display.setTextSize(1);
        display.setCursor(2, 2);
        display.print(Inputs::instance.heldStep);
        display.setCursor(20, 9);
        display.setTextSize(2);
        display.print(seq.getStep(Inputs::instance.heldStep).note);
    }

    drawSteps();
}

#endif //INC_220531_123645_TEENSY41_PLAYPAGE_H
