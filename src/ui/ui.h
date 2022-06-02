//
// Created by theor on 2022-06-01.
//

#ifndef INC_220531_123645_TEENSY41_UI_H
#define INC_220531_123645_TEENSY41_UI_H

#include <array>
#include <Arduino.h>
#include "config.h"

#include "../seq.h"
#include "../display.h"
#include "../uClock.h"

#include "../settings.h"
#include "inputs.h"
#include "playPage.h"

static const unsigned char PROGMEM
CHECKBOX_UNCHECKED[] = {
B00000000, B00000000,
B00000000, B00000000,
B01111111, B11111110,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01000000, B00000010,
B01111111, B11111110,
B00000000, B00000000,
};
static const unsigned char PROGMEM
CHECKBOX_CHECKED[] = {
B00000000, B00000000,
B00000000, B00000000,
B01111111, B11111110,
B01000000, B00000010,
B01011000, B00011010,
B01001100, B00110010,
B01000110, B01100010,
B01000011, B11000010,
B01000001, B10000010,
B01000011, B11000010,
B01000110, B01100010,
B01001100, B00110010,
B01011000, B00011010,
B01000000, B00000010,
B01111111, B11111110,
B00000000, B00000000,
};

static const unsigned char PROGMEM bitmap_arrow_6_12[] = {
B01000000,
B01100000,
B01110000,
B01111000,
B01111100,
B01111110,
B01111100,
B01111000,
B01110000,
B01100000,
B01000000,
B00000000,
};

enum class MenuState {
    Play,
    Settings
};
static MenuState menuState = MenuState::Play;


void printMenuState() {
    display.setCursor(SCREEN_WIDTH - 8 * 8, SCREEN_HEIGHT - 8);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    switch (menuState) {
        case MenuState::Play:
            display.print(F("Play"));
            break;
        case MenuState::Settings:
            display.print(F("Settings"));
            break;
        default:
            display.print((int) menuState);
            break;
    }
}

enum class Action {
    None, // no changes on this I/O control
    Summary,
    Focus,
};
using GetTitle = const char *(*)();
using Checker = Action(*)();
struct MenuOption {
    GetTitle getTitle;
    Checker draw;
//    MenuOption(const __FlashStringHelper * title) {
//        this->title = title;
//    }
};

bool editingValue = false;
int selectedMenuItem = 0;
byte tempo = 55;

template<std::size_t N>
void drawRollMenu(std::array <MenuOption, N> options, int selected) {
    size_t uselected = (size_t)(min(max(selected, 0), (int) options.size() - 1));
    display.setTextWrap(false);
    display.setTextSize(2);
    display.setCursor(0, 0);
    int y = 2;
    int x = 2;
    display.setCursor(x, y + 1);
    const char *title = options[uselected].getTitle();
//    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.print(title);
    y += 24;
    if (editingValue) {
        display.setCursor(x, y + 1);
        display.drawBitmap(x, y+1, bitmap_arrow_6_12, 6, 12, COLOR_WHITE);
//        display.fillRect(x, y, strlen(title) * 12, 18, SSD1306_WHITE);
//        display.setTextColor(SSD1306_BLACK);
    }
//    else
//        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(x + 14, y + 1);
    if (options[uselected].draw != nullptr)
        options[uselected].draw();

    display.setFont(nullptr);
}

template<std::size_t N>
void drawMenu(std::array <String, N> options, int selected) {
    size_t uselected = (size_t)(min(max(selected, 0), (int) options.size() - 1));
    display.setTextWrap(false);
    display.setTextSize(2);
    display.setCursor(0, 0);
    int y = 0;
    for (size_t i = 0; i < options.size(); ++i) {
        int x = 8;
        display.setCursor(x, y + 1);
        if (i == uselected)
            display.fillRect(x, y, options[i].length() * 6, 9, SSD1306_WHITE);
        display.setTextColor(i == uselected ? SSD1306_BLACK : SSD1306_WHITE,
                             i == uselected ? SSD1306_WHITE : SSD1306_BLACK);
        display.print(options[i]);
        y += 16;
    }
    display.setFont(nullptr);
}


void update() {

//    Serial.println((int) Inputs::instance.shift.event);
    switch (Inputs::instance.shift.event) {
        case controlino::Button::Event::ClickPress:
            Serial.print(F("shift click press"));
            menuState = menuState == MenuState::Settings ? MenuState::Play : MenuState::Settings;
            break;
        default:
            break;
    }
    switch (menuState) {
        case MenuState::Play:
        {
            for (size_t i = 0; i < STEP_COUNT; i++) {
                switch (Inputs::instance.steps[i].event) {
                    case controlino::Button::Event::Click:
                        seq.toggleStep(i);
                        break;
                    case controlino::Button::Event::Down:
                        Inputs::instance.heldStep = i;
                        break;
                    case controlino::Button::Event::Up:
                        Inputs::instance.heldStep = -1;
                        break;
                    default:
                        break;
                }
            }

            int delta = Inputs::instance.encoderDelta(0);
            if (delta != 0) {
                if (Inputs::instance.heldStep != -1) {
                    Step step = seq.getStep(Inputs::instance.heldStep);
                    step.note = max(0, min(80, step.note + delta));
                    seq.setStep(Inputs::instance.heldStep, step);
                } else {
                    Pattern &p = seq.getPattern();
                    p.note = max(0, min(80, p.note + delta));
                }
            }
            break;
        }
        case MenuState::Settings:
            if (Inputs::instance.encoder1.event == controlino::Button::Event::Click) {
                editingValue = !editingValue;
                if (!editingValue)
                    Inputs::instance.editingValueDelta = 0;
            }
            if (!editingValue)
                selectedMenuItem += Inputs::instance.encoderDelta(0);
            else
                Inputs::instance.editingValueDelta = Inputs::instance.encoderDelta(0);
            break;
        default:
            break;
    }
    if (Inputs::instance.playPause.event == controlino::Button::Event::Click)
        uClock.pause();
    else if (Inputs::instance.playPause.event == controlino::Button::Event::Press)
        uClock.stop();
}


static std::array<MenuOption, 3>
        SettingsMenuOptions = {{
                                       {([] { return ("Tempo"); }),
                                        [] {
                                            tempo += Inputs::instance.editingValueDelta;
                                            display.print(tempo);
                                            uClock.setTempo(tempo);
                                            return Action::None;
                                        }},
                                       {([] { return ("Invert"); }),
                                        []() {
                                            bool val = settings.invertLcd();
                                            Serial.print(val);
                                            if (editingValue) {
                                                Serial.println();
                                                Serial.println(!val);
                                                editingValue = false;
                                                val = !val;
                                                display.invertDisplay(settings.invertLcd(val));
                                            }
                                            display.drawBitmap(display.getCursorX(), display.getCursorY(),
                                                               val ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED, 16, 16,
                                                               SSD1306_WHITE);
                                            return Action::None;
                                        }},
                                       {([] { return ("ERT"); }), [] { return Action::None; }}
                               }};

void draw() {
    switch (menuState) {
        case MenuState::Play: {
            drawPlayPage();
            break;
        }
        case MenuState::Settings:
            drawRollMenu(SettingsMenuOptions, selectedMenuItem);
            break;
        default:
            break;
    }
    printMenuState();
    // display.setCursor(0, 0);
    // display.setTextSize(2);
    // display.setTextColor(SSD1306_WHITE);

    // for (unsigned i = 0; i < ENCODER_COUNT; i++)
    // {
    //     int value = encoders[i].getValue(Inputs::instance.encoderStep());
    //     if (i == 1 && value != uClock.getTempo())
    //         uClock.setTempo(value);

    //     drawCircleGauge(16 + (i)*32, 15, 15, encoders[i].get01Val(Inputs::instance.encoderStep()));

    //     display.setCursor((i)*32, 34);
    //     sprintf(c, "%2d ", value);
    //     display.print(c);
    // }
    // byte beat = (b % 16);
    // drawCircleGauge(96, 15, 15, b / 16.0f);

    // display.fillCircle(96, 15, 2 + 6 * (beat) / 16.0f, SSD1306_WHITE);
    // display.setCursor(96, 34);
    // display.print(beat + 1);
}

#endif //INC_220531_123645_TEENSY41_UI_H
