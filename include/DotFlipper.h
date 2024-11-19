#include <Adafruit_GFX.h>

const char asciiBytesArray[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
const char test01[] = {
    0x02, 0x31, 0x31, 0x35, 0x34, 0x30, 0x30, 0x30, 
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x36, 
    0x30, 0x36, 0x30, 0x36, 0x30, 0x37, 0x46, 0x37, 
    0x46, 0x37, 0x46, 0x36, 0x30, 0x36, 0x30, 0x36, 
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x34, 
    0x36, 0x34, 0x46, 0x34, 0x46, 0x34, 0x39, 0x34, 
    0x39, 0x37, 0x39, 0x33, 0x39, 0x33, 0x31, 0x30, 
    0x30, 0x30, 0x30, 0x32, 0x31, 0x32, 0x44, 0x32, 
    0x44, 0x33, 0x46, 0x33, 0x46, 0x30, 0x30, 0x30, 
    0x30, 0x32, 0x30, 0x32, 0x30, 0x33, 0x46, 0x33, 
    0x46, 0x32, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 
    0x33, 0x30, 0x33, 0x30, 0x33, 0x30, 0x33, 0x30, 
    0x37, 0x30, 0x46, 0x30, 0x46, 0x31, 0x46, 0x31, 
    0x46, 0x31, 0x45, 0x31, 0x43, 0x30, 0x38, 0x30, 
    0x34, 0x30, 0x32, 0x30, 0x31, 0x30, 0x30, 0x30, 
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x33, 0x30, 
    0x37, 0x30, 0x46, 0x30, 0x46, 0x31, 0x46, 0x31, 
    0x46, 0x31, 0x46, 0x31, 0x46, 0x31, 0x46, 0x31, 
    0x46, 0x30, 0x39, 0x30, 0x30, 0x30, 0x30, 0x30, 
    0x30, 0x30, 0x30, 0x30, 0x30, 0x03, 0x42, 0x34
};
const char test02[] = {
    0x02, 0x31, 0x31, 0x35, 0x34, 0x37, 0x46, 0x37, 
    0x46, 0x37, 0x46, 0x37, 0x46, 0x37, 0x46, 0x31, 
    0x46, 0x31, 0x46, 0x31, 0x46, 0x30, 0x30, 0x30, 
    0x30, 0x30, 0x30, 0x31, 0x46, 0x31, 0x46, 0x31, 
    0x46, 0x37, 0x46, 0x37, 0x46, 0x37, 0x46, 0x33, 
    0x39, 0x33, 0x30, 0x33, 0x30, 0x33, 0x36, 0x33, 
    0x36, 0x30, 0x36, 0x34, 0x36, 0x34, 0x45, 0x37, 
    0x46, 0x37, 0x46, 0x35, 0x45, 0x35, 0x32, 0x35, 
    0x32, 0x34, 0x30, 0x34, 0x30, 0x37, 0x46, 0x37, 
    0x46, 0x35, 0x46, 0x35, 0x46, 0x34, 0x30, 0x34, 
    0x30, 0x35, 0x46, 0x35, 0x46, 0x37, 0x46, 0x37, 
    0x46, 0x37, 0x46, 0x37, 0x46, 0x37, 0x46, 0x37, 
    0x46, 0x37, 0x46, 0x37, 0x46, 0x37, 0x45, 0x37, 
    0x43, 0x37, 0x43, 0x37, 0x43, 0x37, 0x43, 0x37, 
    0x38, 0x37, 0x30, 0x37, 0x30, 0x36, 0x30, 0x36, 
    0x30, 0x36, 0x31, 0x36, 0x33, 0x37, 0x37, 0x37, 
    0x42, 0x37, 0x44, 0x37, 0x45, 0x37, 0x46, 0x37, 
    0x46, 0x37, 0x46, 0x37, 0x46, 0x37, 0x43, 0x37, 
    0x38, 0x37, 0x30, 0x37, 0x30, 0x36, 0x30, 0x36, 
    0x30, 0x36, 0x30, 0x36, 0x30, 0x36, 0x30, 0x36, 
    0x30, 0x37, 0x36, 0x37, 0x46, 0x37, 0x46, 0x37, 
    0x46, 0x37, 0x46, 0x37, 0x46, 0x03, 0x36, 0x42
};


class DotFlipper : public Adafruit_GFX {
    public:
        DotFlipper();
        boolean begin();
        void drawPixel(int16_t x, int16_t y, uint16_t color);
        void clear(uint8_t color);
        void invert(void);
        void displayText(String text);
        void displaySmallText(String text);
        uint8_t *getDisplayBuffer(void);
        void flushToFlipdot(void);

        ~DotFlipper(void);
    
    private:
        uint8_t *displayBuffer;
        uint16_t pixelWidth = 84;
        uint16_t pixelHeight = 7;
        char *serialBuffer;
        uint16_t serialBufferLenght = 176;
};