#include "UserManager.h"
#include "User.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#ifdef __linux__
    #include <pigpio.h>
#endif

#define LED_PIN 18  // change this to whatever pin you use

int main() {
    // --- --- --- STARTUP SEQUENCE --- --- ---
    #ifdef __linux__
    if (gpioInitialise() < 0) {
        std::cout << "Failed to initialise pigpio!" << std::endl;
        return 1;
    }
    #endif

    #ifdef __linux__
    gpioSetMode(LED_PIN, PI_OUTPUT);  // set pin as output
    #endif

    UserManager user_manager = UserManager{"../res/test_data.csv"};
    user_manager.printUsers();

    bool run = true;

    // --- --- --- PROGRAM LOOP --- --- ---
    #ifdef __linix__
    while (run) {
        gpioWrite(LED_PIN, 1);  // pin ON
        std::this_thread::sleep_for(std::chrono::seconds(1));
        gpioWrite(LED_PIN, 0);  // pin OFF
        std::this_thread::sleep_for(std::chrono::seconds(1));              // wait 1 second
    }
    
    gpioTerminate();  // cleanup pigpio on exit
    #endif

    return 0;
}