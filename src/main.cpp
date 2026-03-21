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
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#ifdef __linux__
    #include <pigpio.h>
#endif

#define INPUT_PIN 18  // change this to whatever pin you use
#define RESET_PIN 16  // change this to whatever pin you use

std::atomic<ClCommand> cl_command = ClCommand{0};


void print_gui(const std::string& str) {
    std::cout << str << std::endl;
}

void guiThread(std::atomic<bool>& run) {
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window* window = SDL_CreateWindow(
        "BarAqua",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 480,
        SDL_WINDOW_SHOWN
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // GUI LOOP
    SDL_Event event;
    while (run) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) run = false;
        }

        // clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // draw a rectangle
        SDL_Rect rect = {100, 100, 200, 50};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

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
        if (input == "p") {
            cl_command = ClCommand::PRINT;
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

    std::thread gui(
        guiThread,
        std::ref(run)
    );

    while (run) {
        // --- --- --- HARDWARE INTERFACE --- --- ---
        if (gpioRead(RESET_PIN) && !reset_pressed) {
            print_gui("Value at: 0");
            value = 0;
            reset_pressed = true;
        }
        if (!gpioRead(RESET_PIN) && reset_pressed) {
            reset_pressed = false;

        }

        if (gpioRead(INPUT_PIN) && !input_pressed) {
            value += 35;
            print_gui("Value at: " + std::to_string(value));
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
            
            case ClCommand::PRINT:
                user_manager.printUsers();
                cl_command = ClCommand::NONE;
                break;
            
            case ClCommand::RFID_SCANNED:
                std::string rfid_data;
                {
                    std::lock_guard<std::mutex> lock(cl_data_mutex);
                    rfid_data = cl_data;
                }

                // print_gui("Card scanned! RFID was: " + rfid_data);

                try {
                    User& selected_user = user_manager.getUser(rfid_data);

                    if (value == 0) {
                        print_gui("You have currently spent: " + std::to_string(selected_user.getSpending()));
                    } else {
                        print_gui("Charged: " + std::to_string(value));
                    }

                    selected_user.addSpending(value);
                    

                } catch (std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
                
                
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
