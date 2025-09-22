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
  lcd.print("initializing...");
}

void lcdClear() {
  lcd.clear();
}

void lcdClearLine(int line) {
  lcd.setCursor(0, line);
  lcd.print("                    ");
}