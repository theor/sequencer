//
// Created by theor on 2022-06-01.
//

#ifndef INC_220531_123645_TEENSY41_CONFIG_H
#define INC_220531_123645_TEENSY41_CONFIG_H

const int STEP_COUNT = 8;
const int PAGE_COUNT = 1;

#define ENCODER_COUNT 2

#define MUX1_EN 34
#define MUX1_SIG 35
#define MUX1_S0 41
#define MUX1_S1 40
#define MUX1_S2 37
#define MUX1_S3 36

#include <WString.h>
static const __FlashStringHelper* __globalError;
static bool __halt;
#define ASSERT(x) if(!(x)) { \
__halt = true;               \
__globalError = F("ASSERT FAILED: " #x);\
Serial.print(F("ASSERT FAILED: " #x)); \
}

#define CHECK_HALT if(__halt) { \
        display.clearDisplay(); \
        display.setTextWrap(true); \
        display.setCursor(2,2);\
        display.print(__globalError); \
        display.display();      \
        return;\
}

#endif //INC_220531_123645_TEENSY41_CONFIG_H
