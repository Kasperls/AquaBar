#include "userManager.h"
#include "user.h"
#include "command.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>


#ifdef __linux__
    #include <pigpio.h>
#endif

#define INPUT_PIN 18  // change this to whatever pin you use
#define RESET_PIN 16  // change this to whatever pin you use

std::atomic<ClCommand> cl_command = ClCommand{0};


void inputThread(
    std::atomic<bool>& run, 
    std::atomic<ClCommand>& cl_command,
    std::mutex& cl_data_mutex,
    std::string& cl_data
) {
    std::string input;
    while (run) {
        std::getline(std::cin, input);
        // std::cout << input << std::endl;
        if (input == "q") {
            cl_command = ClCommand::QUIT;
            run = false;
        }
        if (input.size() == 10) {
            {
                std::lock_guard<std::mutex> lock(cl_data_mutex);
                cl_data = input;
            }
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

    std::atomic<bool> run = true;
    std::atomic<ClCommand> cl_command = ClCommand::NONE;
    std::string cl_data;
    std::mutex cl_data_mutex;

    // --- --- --- PROGRAM LOOP --- --- ---
    std::thread input(
        inputThread, 
        std::ref(run), 
        std::ref(cl_command),
        std::ref(cl_data_mutex),
        std::ref(cl_data)
    );

    while (run) {
        // --- --- --- HARDWARE INTERFACE --- --- ---
        if (gpioRead(RESET_PIN) && !reset_pressed) {
            std::cout << "Value reset! " << std::endl;
            value = 0;
            reset_pressed = true;
        }
        if (!gpioRead(RESET_PIN) && reset_pressed) {
            reset_pressed = false;

        }

        if (gpioRead(INPUT_PIN) && !input_pressed) {
            value += 35;
            std::cout << "Value at: " << value << std::endl;
            input_pressed = true;
        }
        if (!gpioRead(INPUT_PIN) && input_pressed) {
            input_pressed = false;

        }
        // --- --- --- COMMAND LINE INTERFACE --- --- ---
        switch (cl_command) {
            case ClCommand::NONE:
                break;

            case ClCommand::QUIT:
                run = false;
                cl_command = ClCommand::NONE;
                break;
            
            case ClCommand::RFID_SCANNED:
                std::string rfid_data;
                {
                    std::lock_guard<std::mutex> lock(cl_data_mutex);
                    rfid_data = cl_data;
                }

                std::cout << "Card scanned! RFID was: " << rfid_data << std::endl;

                user_manager.getUser(rfid_data).addSpending(value);
                user_manager.saveData();
                value = 0;

                cl_command = ClCommand::NONE;
                break;
        } 

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    gpioTerminate();  // cleanup pigpio on exit
    input.detach();
    return 0;
}

#endif

#ifndef __linux__
int main() {
    UserManager user_manager = UserManager{"../res/test_data.csv"};
    user_manager.printUsers();
}
#endif
