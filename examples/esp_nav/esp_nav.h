#ifndef ESP_NAV_H
#define ESP_NAV_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ChronosESP32.h>
#include "pinout.h"
#include "button.h"

// Standard font size
#define STD_FONT 2

// Function Declarations
void updateDisplay();
void drawClockFace();
void drawHand(float angle, int length, uint16_t color, int width);
void drawNavigation();
void drawDirectionArrow(const String &direction);
int estimateProgress(const String &distance);
void drawProgressBar(int percent);
void drawCenteredText(const String &text, uint16_t color, int yPos = -1, uint8_t font = STD_FONT);
void drawArcText(const String &text, int radius, float startAngle, float endAngle, uint16_t color);
void drawWrappedText(const String &text, int yPos, int maxWidth, int maxLines, uint16_t color, uint8_t font = STD_FONT);
void connectionCallback(bool state);
void configCallback(Config config, uint32_t a, uint32_t b);
void handleButtonPress(v_btn_t buttonAction);

#endif // ESP_NAV_H