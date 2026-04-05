#include "userManager.h"
#include "user.h"
#include "command.h"
#include "gui.h"

#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

#include <fstream>
#include <iostream>
#include <sstream>

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pigpio.h>

#define PIN_VALUE_35 26
#define PIN_VALUE_20 13
#define PIN_VALUE_05 6
#define RESET_PIN 19

std::atomic<ClCommand> cl_command = ClCommand{0};

void print_gui(const std::string &str)
{
    std::cout << str << std::endl;
}

// !!! function written by claude.ai !!!
std::string keycodeToChar(int code)
{
    switch (code)
    {
    case KEY_0:
        return "0";
    case KEY_1:
        return "1";
    case KEY_2:
        return "2";
    case KEY_3:
        return "3";
    case KEY_4:
        return "4";
    case KEY_5:
        return "5";
    case KEY_6:
        return "6";
    case KEY_7:
        return "7";
    case KEY_8:
        return "8";
    case KEY_9:
        return "9";
    default:
        return "";
    }
};

void inputThread(
    std::atomic<bool> &run,
    std::atomic<ClCommand> &cl_command,
    std::mutex &cl_data_mutex,
    std::string &cl_data)
{

    int fd = open("/dev/input/by-id/usb-IC_Reader_IC_Reader_08FF20171101-event-kbd", O_RDONLY); // change to your device
    if (fd < 0)
    {
        std::cout << "Failed to open input device!" << std::endl;
        return;
    }
    else
    {
        std::cout << "Opened the input device!" << std::endl;
    }

    std::string buffer = "";
    struct input_event ev;

    // !!! loop written by claude.ai !!!
    while (run)
    {
        read(fd, &ev, sizeof(ev));

        if (ev.type == EV_KEY && ev.value == 1)
        {
            if (ev.code == KEY_ENTER)
            {
                std::cout << "Buffer: " << buffer << " size: " << buffer.size() << std::endl;
                if (buffer.size() == 10)
                {
                    {
                        std::lock_guard<std::mutex> lock(cl_data_mutex);
                        cl_data = buffer;
                    }
                    cl_command = ClCommand::RFID_SCANNED;
                }
                buffer = "";
            }
            else
            {
                buffer += keycodeToChar(ev.code);
            }
        }
    }
    close(fd);
}

int main()
{
    // --- --- --- STARTUP SEQUENCE --- --- ---
    int gpio_result = gpioInitialise();
    std::cout << "GPIO init result: " << gpio_result << std::endl;
    if (gpio_result < 0)
    {
        std::cout << "Failed to initialise pigpio!" << std::endl;
        return 1;
    }

    gpioSetMode(PIN_VALUE_35, PI_INPUT); // set pin as output
    gpioSetPullUpDown(PIN_VALUE_35, PI_PUD_UP);
    gpioSetMode(PIN_VALUE_20, PI_INPUT);
    gpioSetPullUpDown(PIN_VALUE_20, PI_PUD_UP);
    gpioSetMode(PIN_VALUE_05, PI_INPUT);
    gpioSetPullUpDown(PIN_VALUE_05, PI_PUD_UP);
    gpioSetMode(RESET_PIN, PI_INPUT);
    gpioSetPullUpDown(RESET_PIN, PI_PUD_UP);

    std::cout << "Running on RP" << std::endl;

    UserManager user_manager = UserManager{"/home/piaqua/Desktop/AquaBar/res/data.csv"};
    // user_manager.printUsers();

    int value = 0;
    bool input_35_pressed = false;
    bool input_20_pressed = false;
    bool input_05_pressed = false;
    bool reset_pressed = false;

    std::atomic<bool> run = true;

    // command line thread variables
    std::atomic<ClCommand> cl_command = ClCommand::NONE;
    std::string cl_data;
    std::mutex cl_data_mutex;

    // SDL2 gui thread variables
    std::atomic<GuiCommand> gui_command = GuiCommand::NONE;
    // ADD GUIBIG AND GUISMALL VARIABLES TO MAKE THE GUI MORE PRETTY
    std::string gui_data_small = "Trykk på en knapp for å velge verdi";
    std::string gui_data_big = "Velkommen!";
    std::mutex gui_data_mutex;
    // --- --- --- PROGRAM LOOP --- --- ---
    std::thread input(
        inputThread,
        std::ref(run),
        std::ref(cl_command),
        std::ref(cl_data_mutex),
        std::ref(cl_data));

    std::thread gui(
        guiThread,
        std::ref(run),
        std::ref(gui_command),
        std::ref(gui_data_mutex),
        std::ref(gui_data_big),
        std::ref(gui_data_small));

    while (run)
    {
        std::cout << "Inside the loop" << std::endl;
        {
            std::lock_guard<std::mutex> lock(gui_data_mutex);
            if (gui_data_big == "-----")
            {
                std::string value_string = "Sum: " + std::to_string(value);
                gui_data_big = value_string;
                gui_data_small = "Bruk knappene for å velge ønsket sum";
            }
        }

        // --- --- --- HARDWARE INTERFACE --- --- ---
        bool pin_reset = gpioRead(RESET_PIN);
        if (!pin_reset && !reset_pressed)
        {
            std::string value_string_big = "Sum: 0";
            std::string value_string_small = "Bruk knappene for å velge ønsket sum";
            print_gui(value_string_big);
            {
                std::lock_guard<std::mutex> lock(gui_data_mutex);
                gui_data_big = value_string_big;
                gui_data_small = value_string_small;
            }
            gui_command = GuiCommand::DRAW_VALUE;

            value = 0;
            reset_pressed = true;
        }
        if (pin_reset && reset_pressed)
        {
            reset_pressed = false;
        }

        bool pin_35 = gpioRead(PIN_VALUE_35);
        if (!pin_35 && !input_35_pressed)
        {
            value += 35;
            std::string value_string = "Sum: " + std::to_string(value);
            print_gui(value_string);
            {
                std::lock_guard<std::mutex> lock(gui_data_mutex);
                gui_data_big = value_string;
                gui_data_small = "Scan kortet ditt for å krite";
            }
            gui_command = GuiCommand::DRAW_VALUE;

            input_35_pressed = true;
        }
        if (pin_35 && input_35_pressed)
        {
            input_35_pressed = false;
        }

        bool pin_20 = gpioRead(PIN_VALUE_20);
        if (!pin_20 && !input_20_pressed)
        {
            value += 20;
            std::string value_string = "Sum: " + std::to_string(value);
            print_gui(value_string);
            {
                std::lock_guard<std::mutex> lock(gui_data_mutex);
                gui_data_big = value_string;
                gui_data_small = "Scan kortet ditt for å krite";
            }
            gui_command = GuiCommand::DRAW_VALUE;

            input_20_pressed = true;
        }
        if (pin_20 && input_20_pressed)
        {
            input_20_pressed = false;
        }

        bool pin_05 = gpioRead(PIN_VALUE_05);
        if (!pin_05 && !input_05_pressed)
        {
            value += 5;
            std::string value_string = "Sum: " + std::to_string(value);
            print_gui(value_string);
            {
                std::lock_guard<std::mutex> lock(gui_data_mutex);
                gui_data_big = value_string;
                gui_data_small = "Scan kortet ditt for å krite";
            }
            gui_command = GuiCommand::DRAW_VALUE;

            input_05_pressed = true;
        }
        if (pin_05 && input_05_pressed)
        {
            input_05_pressed = false;
        }
        // --- --- --- COMMAND LINE INTERFACE --- --- ---
        switch (cl_command)
        {
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

            try
            {
                User &selected_user = user_manager.getUser(rfid_data);

                if (selected_user.getName() == "Reset")
                {

                    std::ifstream data_csv(user_manager.getPath());
                    std::string line;
                    std::string data_csv_string;

                    if (!data_csv.is_open())
                    {
                        std::cout << "Failed to open csv file!" << std::endl;
                    }

                    while (std::getline(data_csv, line))
                    {
                        // std::cout << line << std::endl;
                        data_csv_string += line + "\n";
                    }

                    gui_command = GuiCommand::DRAW_END;
                    std::string backup_command = "python3 /home/piaqua/Desktop/AquaBar/python/backup_file.py \"" + data_csv_string + "\"";
                    std::string mail_command = "python3 /home/piaqua/Desktop/AquaBar/python/mail_automation.py \"Aqua Bar system test\" \"" + data_csv_string + "\" kaspel@samfundet.no";
                    system(mail_command.c_str());
                    system(backup_command.c_str());
                    user_manager.saveData(true);
                }
                else if (selected_user.isBlocked())
                {
                    gui_command = GuiCommand::DRAW_BLOCKED;
                }
                else if (value == 0)
                {
                    std::string user_spending_string = "I dag har du brukt: " + std::to_string(selected_user.getSpending());
                    print_gui(user_spending_string);
                    // First lock the mutex, then send gui draw command
                    {
                        std::lock_guard<std::mutex> lock(gui_data_mutex);
                        gui_data_big = "Legg inn hvor mye du vil krite!";
                        gui_data_small = user_spending_string;
                    }
                    gui_command = GuiCommand::DRAW_SPENDING;
                }
                else
                {
                    std::string value_string = "Du krysset: " + std::to_string(value);
                    print_gui(value_string);
                    // First lock the mutex, then send gui draw command
                    {
                        std::lock_guard<std::mutex> lock(gui_data_mutex);
                        gui_data_big = value_string;
                        gui_data_small = "Krysset på: " + selected_user.getName();
                    }
                    gui_command = GuiCommand::DRAW_CHECKOUT;
                    selected_user.addSpending(value);
                }

                // A little cursed to try the whole code block, but to lazy to fix
            }
            catch (std::runtime_error &e)
            {
                std::cout << e.what() << std::endl;
                gui_command = GuiCommand::DRAW_UNKOWN;

                user_manager.saveData();
                value = 0;

                cl_command = ClCommand::NONE;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    }
    gpioTerminate(); // cleanup pigpio on exit
    input.detach();
    gui.detach();
    return 0;
}

#ifndef __linux__
int main()
{
    UserManager user_manager = UserManager{"../res/test_data.csv"};
    user_manager.printUsers();
}
#endif
