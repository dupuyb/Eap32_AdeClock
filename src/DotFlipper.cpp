
#include <Adafruit_GFX.h>
#include "DotFlipper.h"
#include <Font5x7FixedMono.h>
#include <Fonts/TomThumb.h>

// Hardware Serial 2 pins
#define RXD2 16
#define TXD2 17
 

DotFlipper::DotFlipper() : Adafruit_GFX(pixelWidth, pixelHeight) {

    // Serial init
    Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2);

}

boolean DotFlipper::begin() {

    if((!displayBuffer) && !(displayBuffer = (uint8_t *)malloc(pixelWidth * pixelHeight) )) {
        return false;
    }

    if((!serialBuffer) && !(serialBuffer = (char *)malloc(serialBufferLenght) )) {
        return false;
    }

    clear(0x00);
    flushToFlipdot();

    return true;
}

void DotFlipper::clear(uint8_t color) {
    for (int i=0; i<(pixelWidth * pixelHeight); i++) {
        displayBuffer[i] = color;
    }
}

void DotFlipper::invert() {
    for (int i=0; i<(pixelWidth * pixelHeight); i++) {
        displayBuffer[i] = displayBuffer[i]==0x00?0xFF:0x00;
    }
}

void DotFlipper::displaySmallText(String text) {
    clear(0x00);
    GFXcanvas1 canvas(84, 7); // pixel canvas
    canvas.setFont(&TomThumb);
    canvas.setRotation(2);
    canvas.setCursor(0,6);
    canvas.print(text);
    drawBitmap(0, 0, canvas.getBuffer(), 84, 7, 0xff, 0x00); // Copy to screen
    flushToFlipdot();
}

void DotFlipper::displayText(String text) {
    clear(0x00);
    GFXcanvas1 canvas(84, 7); // pixel canvas
    canvas.setFont(&Font5x7FixedMono);
    canvas.setRotation(2);
    canvas.setCursor(0,7);
    canvas.print(text);
    drawBitmap(0, 0, canvas.getBuffer(), 84, 7, 0xff, 0x00); // Copy to screen
    flushToFlipdot();
}

void DotFlipper::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Serial.printf("x=%d, y=%d, c=%d\n", x, y, color);
    if (x>=pixelWidth || y>=pixelHeight) return;
    if (x<0 || y<0) return;
    displayBuffer[ (y*pixelWidth)+x ] = color;
    
    // if (color != 0x00) Serial.write('#');
    // else Serial.write('O');
}

void DotFlipper::flushToFlipdot() {
    uint8_t index = 0;
    uint8_t abyte = 0x00;
    uint32_t sum = 0;

    // header
    serialBuffer[index++] = 0x02;   // start
    serialBuffer[index++] = 0x31;   // address
    serialBuffer[index++] = 0x31;   // address
    serialBuffer[index++] = 0x35;   // screen resolution
    serialBuffer[index++] = 0x34;   // screen resolution

    for (uint8_t x = 0; x < pixelWidth; x++) {
        abyte = 0x00;
        for (uint8_t y = 0; y < pixelHeight; y++) {
            if (displayBuffer[x+(y*pixelWidth)] != 0x00) {
                abyte = abyte | (0x01<<y);
            }
        }
        serialBuffer[index++] = asciiBytesArray[ (abyte & 0xF0) >> 4];
        serialBuffer[index++] = asciiBytesArray[ (abyte & 0x0F) ];
    }

    // make the CRC
    for (uint8_t i = 0; i < index; i++) {
        sum += serialBuffer[i];
    }

    // Start of text (0x02) must be removed,
    // End of text (0x03) must be added
    // -2+3 = 1
    sum++;
    sum = sum & 0xFF;
    uint8_t crc = (sum ^ 0xFF) +1;

    // make the footer
    serialBuffer[index++] = 0x03;
    serialBuffer[index++] = asciiBytesArray[ (crc & 0xF0) >> 4];
    serialBuffer[index++] = asciiBytesArray[ (crc & 0x0F) ];
    
    // Serial.write(serialBuffer, 176);
    Serial2.write(serialBuffer, 176);
}

uint8_t *DotFlipper::getDisplayBuffer() { return displayBuffer; }

DotFlipper::~DotFlipper(void) {
    
    if (displayBuffer) {
        free(displayBuffer);
        displayBuffer = NULL;
    }
    
    if (serialBuffer) {
        free(serialBuffer);
        serialBuffer = NULL;
    }
}