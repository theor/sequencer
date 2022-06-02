
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <array>

#include <Bounce2.h>
//#include <Fonts/FreeMono12pt7b.h>

#include "config.h"

#include "uClock.h"
#include "seq.h"
#include "display.h"
#include "ui/ui.h"


unsigned long m;
char c[10];
const int ledPin = 13;

volatile byte b = 0;

// The callback function wich will be called by Clock each Pulse of 96PPQN clock resolution.
void ClockOut96PPQN(uint32_t tick) {
    const int DIV = 6;
    b = (tick / DIV) % 256;
    if ((tick % DIV) == DIV - 1) {
        // Send MIDI_CLOCK to external gears
        seq.step(b + 1);
        seq.triggerNotes();
        usbMIDI.send_now();
    }
    usbMIDI.sendRealTime(usbMIDI.Clock);
}

// The callback function wich will be called by Clock each Pulse of 16PPQN clock resolution.
//void ClockOut16PPQN(uint32_t tick) {
//}

// The callback function wich will be called when clock starts by using Clock.start() method.
void onClockStart() {
    usbMIDI.sendRealTime(usbMIDI.Start);
}

// The callback function wich will be called when clock stops by using Clock.stop() method.
void onClockStop() {
    usbMIDI.sendRealTime(usbMIDI.Stop);
}

void sendNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
    // Serial.print("Send NoteOn  ");
    // Serial.print(inNumber);
    // Serial.print(" vel ");
    // Serial.println(inVelocity);
    usbMIDI.sendNoteOn(inNumber, inVelocity, inChannel);
}

void sendNoteOff(byte inChannel, byte inNumber, byte inVelocity) {
    // Serial.print("Send NoteOff  ");
    // Serial.print(inNumber);
    // Serial.print(" vel ");
    // Serial.println(inVelocity);
    usbMIDI.sendNoteOff(inNumber, inVelocity, inChannel);
}


void setup() {
    seq.setNoteCallbacks(&sendNoteOn, &sendNoteOff);
    Serial.begin(115200);
    delay(1000);
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.drawPixel(10, 10, SSD1306_WHITE);
    display.display();

    setupInputs();

    pinMode(MUX1_EN, OUTPUT);
    digitalWrite(MUX1_EN, HIGH);

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);

    // Inits the clock
    uClock.init();
    uClock.setClock96PPQNOutput(ClockOut96PPQN);
//    uClock.setClock16PPQNOutput(ClockOut16PPQN);
    uClock.setOnClockStartOutput(onClockStart);
    uClock.setOnClockStopOutput(onClockStop);
    uClock.setTempo(tempo);

    settings.initSettings(
            [](bool invertLcd){ display.invertDisplay(invertLcd); }
            );
}


elapsedMillis sinceLastDraw;

void loop() {
    // seq.triggerNotes();
//    if (sinceLastDraw % 16 == 0)

    if (sinceLastDraw >= 33) {
        pollInputs();
        sinceLastDraw = 0;

        display.clearDisplay();
        update();
        draw();

        display.display();
    }
}