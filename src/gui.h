#pragma once
#include <atomic>
#include <mutex>
#include <string>

enum class GuiCommand {
    NONE,
    QUIT,
    DRAW_SPENDING,   // Known RFID scanned and value at zero
    DRAW_CHECKOUT,   // Known RFID scanned
    DRAW_VALUE,      // Reset and increment value
    DRAW_UNKOWN,     // Unknown RFID scanned
    DRAW_END,        // Reset-RFID scanned
    DRAW_BLOCKED,    // User is blocked
};

// void guiThread(std::atomic<bool>& run, std::atomic<GuiCommand>& gui_command);

void guiThread(
    std::atomic<bool>& run, 
    std::atomic<GuiCommand>& gui_command,
    std::mutex& gui_data_mutex,
    std::string& gui_data_big,
    std::string& gui_data_small
);
