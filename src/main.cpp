#include "userManager.h"
#include "user.h"
#include "command.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>


#ifdef __linux__
    #include <pigpio.h>
#endif

#define INPUT_PIN 18  // change this to whatever pin you use
#define RESET_PIN 16  // change this to whatever pin you use

std::atomic<bool> run = true;
std::atomic<bool> pay = false;
std::atomic<ClCommand> cl_command = ClCommand{0};


void inputThread() {
    std::string input;
    while (run) {
        std::getline(std::cin, input);
        std::cout << input << std::endl;
        if (input == "q") {
            cl_command = ClCommand::QUIT;
        }
        if (input.size() == 10) {
            cl_command = ClCommand::RFID_SCANNED;
        }
    }
}

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


    int value = 0;
    bool input_pressed = false;
    bool reset_pressed = false;

    // --- --- --- PROGRAM LOOP --- --- ---
    std::thread input(inputThread);

    while (run) {
        if (gpioRead(RESET_PIN) && !reset_pressed) {
            std::cout << "Reset pressed!" << std::endl;
            std::cout << "Value reset! " << std::endl;
            value = 0;
            reset_pressed = true;
        }
        if (!gpioRead(RESET_PIN) && reset_pressed) {
            std::cout << "Reset reset!" << std::endl;
            reset_pressed = false;

        }

        if (gpioRead(INPUT_PIN) && !input_pressed) {
            value += 35;
            std::cout << "Button pressed!" << std::endl;
            std::cout << "Value at: " << value << std::endl;
            input_pressed = true;
        }
        if (!gpioRead(INPUT_PIN) && input_pressed) {
            std::cout << "Button reset!" << std::endl;
            input_pressed = false;

        }

        switch () {
            case ClCommand::NONE:
                break;

            case ClCommand::QUIT:
                run = false;
                cl_command = ClCommand::NONE;
                break;
            
            case ClCommand::RFID_SCANNED:
                std::cout << "Card scanned!" << std::endl;
                cl_command = ClCommand::NONE;
                break;
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