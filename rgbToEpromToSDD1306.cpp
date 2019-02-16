#include <Arduino.h>
// RGB Strip
#include "FastLED.h" // Library to control the WS2812 LED Ring
// Write to flash mem
#include <EEPROM.h> // Library needed to access EEPROM
// Dipslay
#include <U8x8lib.h>

// Create a U8x8log object
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// U8x8 constructor for your display (I2C)
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE); //VCC,GRD, SDA, SCL only
U8X8LOG u8x8log;
#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
// **************************************

// Define RGB Strip info
#define NUM_Leds 5   // Literal how many RGB pixels on the strip
#define Switch_Pin 4 // pin -> switch -> GND
#define Data_Pin 5   // Data Pin on LED Strip
CRGB RGB_Strip[NUM_Leds];
// EEProm Location(s)
#define MEMLOC_LEDNUM 1
int SavedNumber;

void displayLog()
{
  u8x8log.print("Mem: ");
  u8x8log.print(MEMLOC_LEDNUM);
  u8x8log.print(" Val: ");
  u8x8log.println(SavedNumber);
}

void storeIntToEprom(int value)
{
  EEPROM.put(MEMLOC_LEDNUM, value);
  displayLog();
}

void updateDisplay(int savedNumber)
{
  FastLED.clear();

  for (int led = 0; led < savedNumber; led++)
  {
    if (led < 2)
      RGB_Strip[led] = CRGB::Red;
    if (led >= 2 & led < 4)
      RGB_Strip[led] = CRGB::Green;
    if (led >= 4)
      RGB_Strip[led] = CRGB::Blue;
  }
  FastLED.setBrightness(50);
  FastLED.show();
}

void fadeall()
{
  for (int i = 0; i < NUM_Leds; i++)
  {
    RGB_Strip[i].nscale8(250);
  }
}

void flushRGBStrip()
{
  FastLED.clear();
  FastLED.setBrightness(75);
  for (int led = 0; led < NUM_Leds; led++)
  {
    if (led < 2)
    {
      RGB_Strip[led] = CRGB::Blue;
      FastLED.show();
      delay(75);
    }

    if (led >= 2 & led < 4)
    {
      RGB_Strip[led] = CRGB::Green;
      FastLED.show();
      delay(50);
    }

    if (led >= 4)
    {
      RGB_Strip[led] = CRGB::Red;
      FastLED.show();
      delay(30);
    }
  }
}

void setupDisplay()
{
  u8x8.begin();
  // Set a suitable font. This font will be used for U8x8log
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  // Start U8x8log, connect to U8x8, set the dimension and assign the static memory
  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  // Set the U8x8log redraw mode
  u8x8log.setRedrawMode(1); // 0: Update screen with newline, 1: Update screen for every char
}

void setup()
{
  setupDisplay();
  flushRGBStrip();

  FastLED.addLeds<NEOPIXEL, Data_Pin>(RGB_Strip, NUM_Leds);

  pinMode(Switch_Pin, INPUT_PULLUP);

  if (EEPROM.read(256) != 123)
  {
    EEPROM.write(256, 123);
    SavedNumber = 0;
  }
  else
  {
    EEPROM.get(MEMLOC_LEDNUM, SavedNumber);
    displayLog();
  }
}

void loop()
{
  if (SavedNumber != 0)
  {
    updateDisplay(SavedNumber);
  }

  // Check Switch
  if (digitalRead(Switch_Pin) == LOW)
  {
    flushRGBStrip();
    SavedNumber = random(1, NUM_Leds + 1);
    storeIntToEprom(SavedNumber);
    updateDisplay(SavedNumber);
  }
}
