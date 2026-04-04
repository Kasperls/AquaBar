#include "ssd1306.h"
#include "font6x8.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <ostream>

extern const uint8_t font6x8[][6];

SSD1306::SSD1306(int address) {
    const char* dev = "/dev/i2c-1";
    fd = open(dev, O_RDWR);
    ioctl(fd, I2C_SLAVE, address);
    memset(buffer, 0, sizeof(buffer));
}

void SSD1306::command(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};
    write(fd, data, 2);
}

void SSD1306::data(uint8_t value) {
    uint8_t data[2] = {0x40, value};
    write(fd, data, 2);
}

void SSD1306::init() {

    command(0xAE); // Display OFF

    command(0xD5); // Set display clock divide ratio/oscillator frequency
    command(0x80); // Suggested default

    command(0xA8); // Set multiplex ratio
    command(0x1F); // 0x1F = 31 => (128x32 panel)

    command(0xD3); // Set display offset
    command(0x00); // No offset

    command(0x40); // Set display start line to 0

    command(0xAD); // Set charge pump
    command(0x8B); // Enable charge pump (Adafruit uses 0x14 or 0x8B depending on panel)

    command(0xA1); // Segment re-map (mirror horizontally if needed)

    command(0xC8); // COM output scan direction (flip vertically if needed)

    command(0xDA); // Set COM pins hardware configuration
    command(0x02); // 0x02 = correct for 128x32

    command(0x81); // Set contrast control
    command(0x8F); // Contrast value (0xCF, 0x8F, or 0x7F all work)

    command(0xD9); // Set pre-charge period
    command(0xF1); // Recommended value

    command(0xDB); // Set VCOMH deselect level
    command(0x40); // Default

    command(0xA4); // Entire display ON (resume RAM content)
    command(0xA6); // Normal display (not inverted)

    // Addressing setup
    command(0x20); // Set Memory Addressing Mode
    command(0x00); // Horizontal addressing mode

    command(0x21); // Set column address
    command(0x00); // Column start
    command(0x7F); // Column end (127)

    command(0x22); // Set page address
    command(0x00); // Page start
    command(0x03); // Page end (0-3 for 128x32)

    command(0xAF); // Display ON
}

void SSD1306::clear() {
    memset(buffer, 0, sizeof(buffer));
}

void SSD1306::drawChar(int x, int y, char c) {
    const uint8_t* f = font6x8[c - 32];
    for (int i = 0; i < 6; i++) {
        buffer[x + i + (y/8) * 128] = f[i];
    }
}

void SSD1306::drawString(int x, int y, const char* str) {
    while (*str) {
        drawChar(x, y, *str);
        x += 6;
        str++;
    }
}

void SSD1306::display() {
    command(0x21); command(0); command(127);
    command(0x22); command(0); command(3);
    for (int i = 0 ; i < sizeof(buffer); i++) {
        data(buffer[i]);
    }
}