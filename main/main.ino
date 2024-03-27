#include "Keyboard.h"
#include <SPI.h>

const int pinLed = 13;
const int chipSelect = 2;

void setup() {
  pinMode(pinLed, INPUT);
  Serial1.begin(9600); // Serial port for UART USB
  while (!Serial1);
  Keyboard.begin(); // Keyboard library
  digitalWrite(pinLed, HIGH);
}

void loop() {
  while (Serial1.available()) {
    Keyboard.print((char) Serial1.read()); // Writing
  }
}
