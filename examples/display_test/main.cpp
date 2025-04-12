#include <Arduino.h>
#include <TFT_eSPI.h>
#include "pinout.h"
#include "button.h" // Include the KNOMI button library

#define STD_FONT 2
#define BUTTON_PIN BOOT_PIN // Use the BOOT_PIN defined in pinout_knomi_v1.h

TFT_eSPI tft = TFT_eSPI();

void handleButtonPress(v_btn_t buttonAction)
{
    tft.setTextDatum(MC_DATUM); // Center text
    tft.setTextPadding(240);    // Ensure text clears properly

    if (buttonAction == BTN_CLICK)
    {
        tft.fillScreen(TFT_GREEN);
        tft.setTextColor(TFT_WHITE, TFT_GREEN);                // Text color with background
        tft.drawString("Button Clicked!", 120, 120, STD_FONT); // Centered text
    }
    else if (buttonAction == BTN_LONG_CLICK)
    {
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.drawString("Long Press Detected!", 120, 120, STD_FONT);
    }
    else if (buttonAction == BTN_DOUBLE_CLICK)
    {
        tft.fillScreen(TFT_BLUE);
        tft.setTextColor(TFT_WHITE, TFT_BLUE);
        tft.drawString("Double Click!", 120, 120, STD_FONT);
    }
    else if (buttonAction == BTN_TRIPLE_CLICK)
    {
        tft.fillScreen(TFT_YELLOW);
        tft.setTextColor(TFT_WHITE, TFT_YELLOW);
        tft.drawString("Triple Click!", 120, 120, STD_FONT);
    }
}

void setup()
{
    Serial.begin(115200);

    // Initialize display
    tft.init();
    tft.setRotation(2);             // Adjust rotation if needed (0-3)
    pinMode(LCD_BL_PIN, OUTPUT);    // Backlight control pin
    digitalWrite(LCD_BL_PIN, HIGH); // Turn backlight ON

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM); // Center text
    tft.setTextPadding(240);
    tft.setTextSize(1);
    tft.drawString("KNOMI Button Test", 120, 120, STD_FONT); // Centered text
    // Initialize the button
    Button_Init(BUTTON_PIN);
}

void loop()
{
    // Check button action
    v_btn_t buttonAction = Button_GetAction(BUTTON_PIN);
    if (buttonAction != BTN_IDLE)
    {
        handleButtonPress(buttonAction);
    }
}