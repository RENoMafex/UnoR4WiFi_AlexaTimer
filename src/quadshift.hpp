#include <Arduino.h>

//overload for using latch pin
void shiftOut(pin_size_t dataPin, pin_size_t clockPin, pin_size_t latchPin, BitOrder bitOrder, uint8_t val) {
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, bitOrder, val);
	digitalWrite(latchPin, HIGH);
}