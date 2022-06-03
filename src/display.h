
#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#define COLOR_BLACK ST7735_BLACK   ///< Draw 'off' pixels
#define COLOR_WHITE ST7735_WHITE   ///< Draw 'on' pixels
#define COLOR_INVERSE 2 ///< Invert pixels
#define SCREEN_WIDTH 160 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels

#define TFT_CS        38
#define TFT_RST        33 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         39 // AO
#define TFT_MOSI         26 //SDA
#define TFT_SCLK         27
//static Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
static Adafruit_ST7735 display = Adafruit_ST7735(&SPI1, TFT_CS, TFT_DC, TFT_RST);
static const float INV_DEG3 = 60.0f / PI;
static const float DEG3 = PI / 60.0f;

void initDisplay(){
    display.initR(INITR_BLACKTAB);
    display.setRotation(1);
}

void fillArc2(int x, int y, float start_angle, int seg_count, int rx, int ry, int w, unsigned int colour)
{
    float seg = DEG3; // Segments are 3 degrees wide = 120 segments for 360 degrees
    float inc = DEG3; // Draw segments every 3 degrees, increase to 6 for segmented ring

    // Calculate first pair of coordinates for segment start
    float sx = cos((start_angle));
    float sy = sin((start_angle));
    uint16_t x0 = sx * (rx - w) + x;
    uint16_t y0 = sy * (ry - w) + y;
    uint16_t x1 = sx * rx + x;
    uint16_t y1 = sy * ry + y;

    // Draw colour blocks every inc degrees
    for (float i = start_angle; i < start_angle + seg * seg_count; i += inc)
    {

        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg));
        float sy2 = sin((i + seg));
        int x2 = sx2 * (rx - w) + x;
        int y2 = sy2 * (ry - w) + y;
        int x3 = sx2 * rx + x;
        int y3 = sy2 * ry + y;

        display.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
        display.fillTriangle(x1, y1, x2, y2, x3, y3, colour);

        // Copy segment end to sgement start for next segment
        x0 = x2;
        y0 = y2;
        x1 = x3;
        y1 = y3;
    }
}

void drawCircleGauge(int cx, int cy, int r, float val01)
{
    display.drawCircle(cx, cy, r, COLOR_WHITE);
    int count = val01 * TWO_PI * INV_DEG3; //* TWO_PI
    fillArc2(cx, cy, 0, count, r, r, 4, COLOR_WHITE);
    // display.drawLine(cx, cy, cx + r * cos(val01 * TWO_PI - HALF_PI), cy + r * sin(val01 * TWO_PI - HALF_PI), SSD1306_WHITE);
}

#endif