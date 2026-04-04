#pragma once
#include <stdint.h>

class SSD1306 {
public:
    SSD1306(int i2c_address = 0x3C);
    void init();
    void clear();
    void drawChar(int x, int y, char c);
    void drawBigChar(int x, int y, char c);
    void drawPixel(int x, int y);
    void drawString(int x, int y, const char* str);
    void drawBigString(int x, int y, const char* str);
    void display();

private:
    int fd;
    uint8_t buffer[128 * 32 / 8];
    void command(uint8_t cmd);
    void data(uint8_t value);
};