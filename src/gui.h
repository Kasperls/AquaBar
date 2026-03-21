#pragma once
#include <atomic>

enum class GuiCommand {
    NONE,
    QUIT,
    DRAW_SPENDING,   // Known RFID scanned and value at zero
    DRAW_CHECKOUT,   // Known RFID scanned
    DRAW_VALUE,      // Reset and increment value
    DRAW_UNKOWN,     // Unknown RFID scanned
};

void guiThread(std::atomic<bool>& run, std::atomic<GuiCommand>& gui_command);
