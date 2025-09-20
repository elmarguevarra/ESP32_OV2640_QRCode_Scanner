#pragma once // A common preprocessor directive to prevent multiple inclusions

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Extern declaration for the global LCD object
extern LiquidCrystal_I2C lcd;

// Function declarations with optional parameters
void lcdInit();
void lcdPrint(const char* message, int line = 0, bool isClear = true);
void lcdClear();
void lcdClearLine(int line);