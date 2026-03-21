#include "UserManager.h"
#include "User.h"

#include <iostream>
#include <string>
#include <unistd.h>  // for sleep()

#include <pigpio.h>

#define LED_PIN 18  // change this to whatever pin you use

int main() {
    // --- --- --- STARTUP SEQUENCE --- --- ---
    if (gpioInitialise() < 0) {
        std::cout << "Failed to initialise pigpio!" << std::endl;
        return 1;
    }

    gpioSetMode(LED_PIN, PI_OUTPUT);  // set pin as output

    UserManager user_manager = UserManager{"../res/test_data.csv"};
    user_manager.printUsers();

    bool run = true;

    // --- --- --- PROGRAM LOOP --- --- ---
    while (run) {
        gpioWrite(LED_PIN, 1);  // pin ON
        sleep(1);               // wait 1 second
        gpioWrite(LED_PIN, 0);  // pin OFF
        sleep(1);               // wait 1 second
    }

    gpioTerminate();  // cleanup pigpio on exit
    return 0;
}