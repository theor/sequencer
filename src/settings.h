//
// Created by theor on 2022-06-01.
//

#ifndef INC_220531_123645_TEENSY41_SETTINGS_H
#define INC_220531_123645_TEENSY41_SETTINGS_H

#include "config.h"
#include <EEPROM.h>

#define SETTINGS_DEFS \
    LAMBDA(0x10, bool, invertLcd)


struct Settings {
#define LAMBDA(ADDR, TYPE, VAR) \
    TYPE VAR() { return get<TYPE>(ADDR); } \
    TYPE VAR(TYPE val) { return update(ADDR, val); }

    SETTINGS_DEFS
#undef LAMBDA

#define LAMBDA(ADDR, TYPE, VAR) \
    void (*init ## VAR)(TYPE),
    void initSettings(
            SETTINGS_DEFS
            bool macroHack = true
            )
    {
#undef LAMBDA
#define LAMBDA(ADDR, TYPE, VAR) init ## VAR(VAR());
        SETTINGS_DEFS
#undef LAMBDA
    }

    template< typename T >
    bool get(int idx) {
        return !!EEPROM.read(idx);
    }
    const bool update(int idx, const bool b) {
        EEPROM.update(idx, b ? 1 : 0);
        return b;
    }
};


static Settings settings;

#endif //INC_220531_123645_TEENSY41_SETTINGS_H
