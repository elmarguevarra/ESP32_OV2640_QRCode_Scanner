#include <lcd.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcdPrint(const char* message, int line, bool isClear) {
  if(isClear)
  {
    lcd.clear();
  }
  lcd.setCursor(0, line);
  lcd.print(message);
}

void lcdInit() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32-S3 Camera");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
}

void lcdScroll(const char* message) {
  lcd.clear();
  int messageLength = strlen(message);
  int lcdWidth = 16;
  int delayTime = 300; // Adjust for scrolling speed

  // If the message is shorter than or equal to the LCD width,
  // just display it.
  if (messageLength <= lcdWidth) {
    lcd.setCursor(0, 0);
    lcd.print(message);
    return;
  }

  // Loop through the message to create the scrolling effect.
  for (int i = 0; i <= messageLength - lcdWidth; i++) {
    // Set the cursor to the beginning of the line.
    lcd.setCursor(0, 0);

    // Print a substring of the message.
    for (int j = 0; j < lcdWidth; j++) {
      lcd.print(message[i + j]);
    }
    // Wait for a moment before displaying the next part.
    delay(delayTime);
  }
}

void lcdClear() {
  lcd.clear();
}