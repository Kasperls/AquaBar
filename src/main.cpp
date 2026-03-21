#include "userManager.h"
#include "user.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#ifdef __linux__
    #include <pigpio.h>
#endif

#define INPUT_PIN 18  // change this to whatever pin you use

#ifdef __linux__

int main() {
    // --- --- --- STARTUP SEQUENCE --- --- ---
    int gpio_result = gpioInitialise();
    std::cout << "GPIO init result: " << gpio_result << std::endl;
    if (gpio_result < 0) {
        std::cout << "Failed to initialise pigpio!" << std::endl;
        return 1;
    }

    gpioSetMode(INPUT_PIN, PI_INPUT);  // set pin as output

    std::cout << "Running on RP" << std::endl;

    UserManager user_manager = UserManager{"../res/test_data.csv"};
    user_manager.printUsers();

    bool run = true;

    int value = 0;
    bool pressed = false;

    // --- --- --- PROGRAM LOOP --- --- ---
    while (run) {
        if (gpioRead(INPUT_PIN) && !pressed) {
            value += 35;
            std::cout << "Button pressed!" << std::endl;
            pressed = true;
        }
        if (!gpioRead(INPUT_PIN) && pressed) {
            std::cout << "Button reset!" << std::endl;
            pressed = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    gpioTerminate();  // cleanup pigpio on exit

    return 0;
}

#endif

#ifndef __linux__
int main() {
    UserManager user_manager = UserManager{"../res/test_data.csv"};
    user_manager.printUsers();
}
#endif