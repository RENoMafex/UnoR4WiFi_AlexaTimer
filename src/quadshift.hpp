#include <Arduino.h>

//overload for using latch pin
void shiftOut(pin_size_t dataPin, pin_size_t clockPin, pin_size_t latchPin, BitOrder bitOrder, uint8_t val) {
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, bitOrder, val);
	digitalWrite(latchPin, HIGH);
}



// characters 'KMFVWXZ' and 'kmfvwxz' are not working.
// special characters ' '"()-<=>^_`' are working.
// converts any single digit number or character into an output for shiftOut()
uint8_t toSevSeg(uint8_t val) {
    switch (val) {
        case  0 : return 0b1111110;
        case  1 : return 0b0110000;
        case  2 : return 0b1101101;
        case  3 : return 0b1111001;
        case  4 : return 0b0110011;
        case  5 : return 0b1011011;
        case  6 : return 0b1011111;
        case  7 : return 0b1110000;
        case  8 : return 0b1111111;
        case  9 : return 0b1111011;
        case ' ': return 0b0000000;
        case '"': return 0b0100010;
        case'\'': return 0b0100000; // '''
        case '(': return 0b1001110;
        case ')': return 0b1111000;
        case '-': return 0b0000001;
        case '0': return 0b1111110;
        case '1': return 0b0110000;
        case '2': return 0b1101101;
        case '3': return 0b1111001;
        case '4': return 0b0110011;
        case '5': return 0b1011011;
        case '6': return 0b1011111;
        case '7': return 0b1110000;
        case '8': return 0b1111111;
        case '9': return 0b1111011;
        case '<': return 0b0001101;
        case '=': return 0b0001001;
        case '>': return 0b0011001;
        case 'A': return 0b1110111;
        case 'B': return 0b0011111; // 'b'
        case 'C': return 0b1001110;
        case 'D': return 0b0111101; // 'd'
        case 'E': return 0b1001111;
        case 'F': return 0b1000111;
        case 'G': return 0b1011110;
        case 'H': return 0b0110111;
        case 'I': return 0b0110000;
        case 'J': return 0b0111100;
        case 'L': return 0b0001110;
        case 'N': return 0b0010101; // 'n'
        case 'O': return 0b1111110;
        case 'P': return 0b1100111;
        case 'Q': return 0b1110011; // 'q'
        case 'R': return 0b0000101; // 'r'
        case 'S': return 0b1011011;
        case 'T': return 0b0001111; // 't'
        case 'U': return 0b0111110;
        case 'Y': return 0b0111011;
        case '[': return 0b1001110;
        case ']': return 0b1111000;
        case '^': return 0b1100010;
        case '_': return 0b0001000;
        case '`': return 0b0100000; // '''
        case 'a': return 0b1110111; // 'A'
        case 'b': return 0b0011111;
        case 'c': return 0b0001101;
        case 'd': return 0b0111101;
        case 'e': return 0b1001111; // 'E'
        case 'f': return 0b1000111; // 'F'
        case 'g': return 0b1011110; // 'G'
        case 'h': return 0b0010111;
        case 'i': return 0b0010000;
        case 'j': return 0b0111100; // 'J'
        case 'l': return 0b0001110; // 'L'
        case 'n': return 0b0010101;
        case 'o': return 0b0011101;
        case 'p': return 0b1100111; // 'P'
        case 'q': return 0b1110011;
        case 'r': return 0b0000101;
        case 's': return 0b1011011; // 'S'
        case 't': return 0b0001111;
        case 'u': return 0b0011100;
        case 'y': return 0b0110011; // 'Y'
        default : return 0b0000000;
    }
}
