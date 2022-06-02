//
// Created by theor on 2022-06-02.
//

#include <Arduino.h>
#include "../config.h"
#include "inputs.h"
#include "FactorEncoder.h"
FactorEncoder encoders[ENCODER_COUNT];

Inputs Inputs::instance;
void setupInputs() {
    encoders[0].begin(28, 27);
    encoders[1].begin(29, 26);
}
void pollInputs() {
    digitalWrite(MUX1_EN, LOW);
    Inputs::instance.shift = __buttonShift.check();
//    if((int)Inputs::instance.shift.event > 0) {
//        Serial.print(F("poll "));
//        Serial.println((int) Inputs::instance.shift.event); }

    Inputs::instance.encoder1 = __buttonEnc1.check();
    Inputs::instance.encoder2 = __buttonEnc2.check();
    Inputs::instance.playPause = __buttonPlayPause.check();

    for (unsigned int i = 0; i < STEP_COUNT; i++)
        Inputs::instance.steps[i] = __buttonSteps[i].check();
    for (unsigned int i = 0; i < ENCODER_COUNT; i++) {
        Inputs::instance.encoderPrevValues[i] = Inputs::instance.encoderValues[i];
        Inputs::instance.encoderValues[i] = encoders[i].getValue(Inputs::instance.encoderStep());
//        Serial.print(Inputs::instance.encoderPrevValues[i]);
//        Serial.print(' ');
//        Serial.print(Inputs::instance.encoderValues[i]);
//        Serial.println(' ');
    }

    digitalWrite(MUX1_EN, HIGH);
}
