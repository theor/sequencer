//
// Created by theor on 2022-06-02.
//

#ifndef INC_220531_123645_TEENSY41_INPUTS_H
#define INC_220531_123645_TEENSY41_INPUTS_H
#include "../src/Controlino/Controlino.h"
// static auto __selector = controlino::Selector(MUX1_S3, MUX1_S2, MUX1_S1, MUX1_S0);
static auto __selector = controlino::Selector(MUX1_S0, MUX1_S1, MUX1_S2, MUX1_S3);
static auto __multiplexer = controlino::Multiplexer(MUX1_SIG, __selector);
static auto __buttonEnc1 = controlino::Button(__multiplexer, 0);
static auto __buttonEnc2 = controlino::Button(__multiplexer, 1);
static auto __buttonPlayPause = controlino::Button(__multiplexer, 2);
static auto __buttonShift = controlino::Button(__multiplexer, 3);
static controlino::Button __buttonSteps[STEP_COUNT] = {
        {__multiplexer, 4},
        {__multiplexer, 5},
        {__multiplexer, 6},
        {__multiplexer, 7},
        {__multiplexer, 8},
        {__multiplexer, 9},
        {__multiplexer, 10},
        {__multiplexer, 11},
};

struct Inputs {
//    int values[ENCODER_COUNT] = {0, 0};
    controlino::Button::State shift;
    controlino::Button::State playPause;
    controlino::Button::State steps[STEP_COUNT];
    controlino::Button::State encoder1;
    controlino::Button::State encoder2;
    int encoderPrevValues[ENCODER_COUNT];
    int encoderValues[ENCODER_COUNT];

    inline int encoderDelta(unsigned int index) { return encoderValues[index] - encoderPrevValues[index]; }

    inline int encoderStep() { return this->shift.pressed ? 10 : 1; }

    int editingValueDelta;

    // holding a step button in sequencer mode shows step details (note, gate, etc)
    int heldStep = -1;
    static Inputs instance;
};

void setupInputs() ;
void pollInputs() ;

#endif //INC_220531_123645_TEENSY41_INPUTS_H
