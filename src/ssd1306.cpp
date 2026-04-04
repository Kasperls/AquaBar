#include "ssd1306.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>

extern const uint8_t font6x8[][6];

SSD1306::SSD1306(int address) {
    const char* dev = "dev/i2c-1";
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
    command(0xAE);
    command(0x20); command(0x00);
    command(0x40);
    command(0xA1);
    command(0xC8);
    command(0x81); command(0x7F);
    command(0xA4);
    command(0xA6);
    command(0xAF);
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