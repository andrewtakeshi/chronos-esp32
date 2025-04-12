// #include <Arduino.h>
#include "esp_nav.h"

// BEGIN Definitions
// Button pin
#define BUTTON_PIN BOOT_PIN
// Colors
#define BACKGROUND TFT_BLACK
#define PRIMARY TFT_WHITE
#define SECONDARY TFT_YELLOW
#define ACCENT TFT_CYAN
#define ALERT TFT_RED
// END Definitions

// BEGIN Variables
TFT_eSPI tft = TFT_eSPI();       /* Display */
ChronosESP32 watch("Knomi Nav"); /* Chronos BLE */
enum DisplayMode                 /* Available display modes*/
{
    CLOCK,
    NAVIGATION,
};
DisplayMode currentMode = CLOCK; /* Display Mode */
Navigation currentNav;           /* Current navigation data */
// END Variables

void setup()
{
    Serial.begin(115200);

    // Initialize display
    tft.init();
    tft.setRotation(0);             // Adjust rotation if needed (0-3)
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

    watch.setConnectionCallback(connectionCallback);
    watch.setConfigurationCallback(configCallback);
    watch.begin();
}

void loop()
{
    watch.loop();

    // Handle button presses
    v_btn_t buttonAction = Button_GetAction(BUTTON_PIN);
    if (buttonAction != BTN_IDLE)
    {
        handleButtonPress(buttonAction);
    }
    // Update display based on current mode
    updateDisplay();
}

// ======================
// DISPLAY FUNCTIONS
// ======================

void updateDisplay()
{
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate > 1000)
    {
        switch (currentMode)
        {
        case NAVIGATION:
            drawNavigation();
            break;
        case CLOCK:
            drawClockFace();
            break;
        default:
            break;
        }
        lastUpdate = millis();
    }
}

void drawClockFace()
{
    tft.fillScreen(BACKGROUND);

    // Clock face
    tft.drawCircle(120, 120, 100, PRIMARY);
    for (int i = 0; i < 12; i++)
    {
        float angle = i * PI / 6;
        int x1 = 120 + 90 * sin(angle);
        int y1 = 120 - 90 * cos(angle);
        int x2 = 120 + 100 * sin(angle);
        int y2 = 120 - 100 * cos(angle);
        tft.drawLine(x1, y1, x2, y2, PRIMARY);
    }

    // Clock hands
    int hour = watch.getHourC();
    int minute = watch.getMinute();
    int second = watch.getSecond();

    drawHand(hour * 30 + minute * 0.5, 50, PRIMARY, 6); // Hour
    drawHand(minute * 6, 70, SECONDARY, 4);             // Minute
    drawHand(second * 6, 90, ALERT, 2);                 // Second

    // AM/PM indicator if in 12-hour mode
    if (!watch.is24Hour())
    {
        tft.setTextDatum(BC_DATUM);
        tft.setTextColor(ACCENT, BACKGROUND);
        tft.drawString(watch.getAmPmC(true), 120, 230, 2);
    }
}

void drawHand(float angle, int length, uint16_t color, int width)
{
    angle = angle * PI / 180;
    int x = 120 + length * sin(angle);
    int y = 120 - length * cos(angle);
    tft.drawLine(120, 120, x, y, color); // width, tft.color565(random(256), random(256), random(256)));
}

// void drawNavigation()
// {
//     tft.fillScreen(BACKGROUND);

//     Serial.println("Navigation Data:");
//     Serial.println(currentNav.directions);
//     Serial.println(currentNav.distance);
//     Serial.println(currentNav.duration);
//     Serial.println(currentNav.title);

//     if (currentNav.active)
//     {
//         // Direction indicator
//         drawDirectionArrow(currentNav.directions);

//         // Distance (top arc)
//         drawArcText(currentNav.distance, 80, -30, 30, SECONDARY);

//         // Time remaining (bottom arc)
//         drawArcText(currentNav.duration, 80, 150, 210, ACCENT);

//         // Current road (center)
//         drawWrappedText(currentNav.title, 110, 200, 20, PRIMARY);

//         // Next instruction
//         drawWrappedText(currentNav.directions, 180, 220, 20, SECONDARY);

//         // Progress bar
//         if (currentNav.distance.length() > 0)
//         {
//             drawProgressBar(estimateProgress(currentNav.distance));
//         }
//     }
//     else
//     {
//         drawCenteredText("No Active Navigation", PRIMARY, -1, STD_FONT);
//     }

//     // Outer compass circle
//     tft.drawCircle(TFT_WIDTH, TFT_HEIGHT, 110, PRIMARY);
// }

void drawNavigation()
{
    tft.fillScreen(BACKGROUND);

    if (currentNav.active)
    {
        Serial.println("Navigation Data:");
        Serial.println("Active: " + String(currentNav.active));
        Serial.println("Directions: " + currentNav.directions);
        Serial.println("Distance: " + currentNav.distance);
        Serial.println("Duration: " + currentNav.duration);
        Serial.println("Title: " + currentNav.title);

        // Display navigation data
        drawCenteredText("Navigation Active", ACCENT, 20, STD_FONT);

        // Display directions
        drawWrappedText("Directions: " + currentNav.directions, 60, 200, 2, PRIMARY, STD_FONT);

        // Display distance
        drawCenteredText("Distance: " + currentNav.distance, SECONDARY, 100, STD_FONT);

        // Display duration
        drawCenteredText("Duration: " + currentNav.duration, ACCENT, 140, STD_FONT);

        // Display title (e.g., current road)
        drawCenteredText("Road: " + currentNav.title, PRIMARY, 180, STD_FONT);

        // Display navigation icon if available
        // if (currentNav.hasIcon)
        // {
        tft.drawBitmap(120 - 24, 200, currentNav.icon, 48, 48, PRIMARY);
        // }
    }
    else
    {
        drawCenteredText("No Active Navigation", ALERT, -1, STD_FONT);
    }
    tft.drawCircle(TFT_WIDTH / 2, TFT_HEIGHT / 2, 110, PRIMARY);
}

void drawDirectionArrow(const String &direction)
{
    if (direction.indexOf("left") >= 0)
    {
        tft.fillTriangle(60, 120, 90, 90, 90, 150, ACCENT); // Left arrow
    }
    else if (direction.indexOf("right") >= 0)
    {
        tft.fillTriangle(180, 120, 150, 90, 150, 150, ACCENT); // Right arrow
    }
    else
    {
        tft.fillTriangle(120, 60, 90, 120, 150, 120, ACCENT); // Up arrow
    }
}

int estimateProgress(const String &distance)
{
    // Simple progress estimation - adapt based on your needs
    if (distance.indexOf("km") > 0 || distance.indexOf("mi") > 0)
    {
        return 25; // Long distance
    }
    else if (distance.indexOf("m") > 0)
    {
        return 75; // Medium distance
    }
    return 90; // Short distance
}

void drawProgressBar(int percent)
{
    int startAngle = 90;
    int endAngle = 90 - (360 * percent / 100);
    tft.drawArc(120, 120, 105, 110, startAngle, endAngle, ACCENT, BACKGROUND);
}

// ======================
// TEXT HANDLING
// ======================

void drawCenteredText(const String &text, uint16_t color, int yPos, uint8_t font)
{
    yPos = yPos >= 0 ? yPos : TFT_HEIGHT / 2;
    tft.setTextColor(color, BACKGROUND);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(text, TFT_WIDTH / 2, yPos, font);
}

void drawArcText(const String &text, int radius, float startAngle, float endAngle, uint16_t color)
{
    float angleStep = (endAngle - startAngle) / max(1, (int)text.length());
    tft.setTextColor(color, BACKGROUND);

    for (int i = 0; i < text.length(); i++)
    {
        float angle = (startAngle + (i * angleStep)) * PI / 180;
        int x = 120 + radius * cos(angle);
        int y = 120 + radius * sin(angle);
        tft.drawChar(text[i], x, y, 2);
    }
}

void drawWrappedText(const String &text, int yPos, int maxWidth, int maxLines, uint16_t color, uint8_t font)
{
    String displayText = text;
    tft.setTextColor(color, BACKGROUND);
    tft.setTextDatum(TC_DATUM);

    // Simple line wrapping
    if (tft.textWidth(displayText, font) > maxWidth)
    {
        int splitPos = displayText.length() / 2;
        String line1 = displayText.substring(0, splitPos);
        String line2 = displayText.substring(splitPos);

        tft.drawString(line1, 120, yPos - 10, font);
        tft.drawString(line2, 120, yPos + 10, font);
    }
    else
    {
        tft.drawString(displayText, 120, yPos, font);
    }
}

// ======================
// CHRONOS CALLBACKS
// ======================

void connectionCallback(bool state)
{
    Serial.println("In the connection callback");
    tft.fillScreen(BACKGROUND);
    drawCenteredText(state ? "Connected" : "Disconnected", state ? ACCENT : ALERT, -1, STD_FONT);
    if (state)
    {
        drawCenteredText(watch.getAddress(), PRIMARY, -1, STD_FONT);
    }
    delay(1000);
    updateDisplay();
}

void configCallback(Config config, uint32_t a, uint32_t b)
{
    Serial.println("In the config callback");
    Serial.print("Config type: ");
    Serial.println(config);
    Serial.print("Parameter A: ");
    Serial.println(a);
    Serial.print("Parameter B: ");
    Serial.println(b);

    if (config == CF_NAV_DATA)
    {
        currentNav = watch.getNavigation();
        currentNav.active = a; // Set active flag based on the received data

        if (currentNav.active)
        {
            Serial.println("Navigation is active");
            currentMode = NAVIGATION;
            tft.fillScreen(BACKGROUND);
            drawCenteredText("Navigation Started", ACCENT, -1, STD_FONT);
            delay(1000);
            drawNavigation();
        }
        else
        {
            Serial.println("Navigation is inactive");
            currentMode = CLOCK;
        }
    }
    else if (config == CF_NAV_ICON)
    {
        Serial.println("Navigation icon received");
        currentNav = watch.getNavigation();

        if (currentNav.hasIcon)
        {
            Serial.println("Icon is available");
            // Optionally, print the first few bytes of the icon data for debugging
            for (int i = 0; i < 10; i++)
            {
                Serial.print("0x");
                Serial.print(currentNav.icon[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
        }
        else
        {
            Serial.println("No icon available");
        }
    }
}

// void configCallback(Config config, uint32_t a, uint32_t b)
// {
//     if (config == CF_NAV_DATA)
//     {
//         currentNav = watch.getNavigation();
//         if (a)
//         { // Navigation active
//             currentMode = NAVIGATION;
//             tft.fillScreen(BACKGROUND);
//             drawCenteredText("Navigation Started", ACCENT, -1, STD_FONT);
//             delay(1000);
//         }
//         drawNavigation();
//     }
// }

// ======================
// BUTTON HANDLING
// ======================

void handleButtonPress(v_btn_t buttonAction)
{
    if (buttonAction == BTN_CLICK)
    {
        drawCenteredText("Clock", TFT_WHITE, -1, STD_FONT);
        currentMode = CLOCK;
    }
    else if (buttonAction == BTN_DOUBLE_CLICK)
    {
        drawCenteredText("Navigation", TFT_WHITE, -1, STD_FONT);
        currentMode = NAVIGATION;
    }

    delay(1000);
    updateDisplay();
}