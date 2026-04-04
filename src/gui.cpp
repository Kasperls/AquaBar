#include "gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>
#include <string>
#include <mutex>
#include "ssd1306.h"
#include <unistd.h>
#include <iostream>
#include <ostream>

// !!! GUI boilerplate written by claude.ai !!!

#define WINDOW_W 1920
#define WINDOW_H 1080
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

// OLED screen gpio
// GPIO2 - SDA
// GPIO3 - SCL

// Helper to render text centered at a y position
static void drawTextCentered(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    SDL_Rect dst = {0, y, surface->w, surface->h};
    dst.x = (WINDOW_W - surface->w) / 2;  // center horizontally
    
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

// Helper to draw a filled rounded rectangle (background card)
static void drawBackground(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void guiThread(
    std::atomic<bool>& run, 
    std::atomic<GuiCommand>& gui_command,
    std::mutex& gui_data_mutex,
    std::string& gui_data_big,
    std::string& gui_data_small
) {
    // --- INIT ---
    // SSD1306 init
    std::cout << "before oled init" << std:endl; 
    SSD1306 display(0x3C);
    display.init();
    std::cout << "after oled init" << std:endl; 
    sleep(300);
    display.drawString(0, 0, "hello world");
    sleep(2000);
    display.clear();
    std::cout << "after first draw call" << std:endl; 


    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "BarAqua",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_FULLSCREEN_DESKTOP
    );
    

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font_large = TTF_OpenFont(FONT_PATH, 64);
    TTF_Font* font_medium = TTF_OpenFont(FONT_PATH, 36);
    TTF_Font* font_small = TTF_OpenFont(FONT_PATH, 24);
    TTF_Font* font_tiny = TTF_OpenFont(FONT_PATH, 18);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color black  = {0,   0,   0,   255};
    SDL_Color green  = {50,  200, 100, 255};
    SDL_Color red    = {200, 50,  50,  255};
    SDL_Color blue   = {50,  100, 200, 255};
    SDL_Color orange = {220, 140, 0,   255};
    SDL_Color turk   = {118, 174, 101, 255};

    GuiCommand active_command = GuiCommand::DRAW_VALUE;
    auto command_time = std::chrono::steady_clock::now();

    // --- LOOP ---
    while (run) {
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) run = false;
        }

        // Check for new command
        GuiCommand new_command = gui_command.load();
        if (new_command != GuiCommand::NONE && new_command != GuiCommand::DRAW_VALUE) {
            active_command = new_command;
            command_time = std::chrono::steady_clock::now();
            gui_command = GuiCommand::NONE;
        } else if (new_command == GuiCommand::DRAW_VALUE) {
            active_command = GuiCommand::DRAW_VALUE;
            gui_command = GuiCommand::NONE;
        }

        // Revert to DRAW_VALUE after 2 seconds
        auto elapsed = std::chrono::steady_clock::now() - command_time;
        if (active_command != GuiCommand::DRAW_VALUE &&
            std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 2) {
            active_command = GuiCommand::DRAW_VALUE;
            std::string reset_str_value = "-----";
            {
                std::lock_guard<std::mutex> lock(gui_data_mutex);
                gui_data_big = reset_str_value;
                gui_data_small = reset_str_value;
            }
        }

        // --- DRAW ---
        switch (active_command) {

            case GuiCommand::DRAW_VALUE: {
                // WHEN THE PROGRAM WANTS TO DRAW THE VALUE
                drawBackground(renderer, black);
                std::string data_big;
                std::string data_small;
                {
                    std::lock_guard<std::mutex> lock(gui_data_mutex);
                    data_big = gui_data_big;
                    data_small = gui_data_small;
                }
                drawTextCentered(renderer, font_medium, data_big, 100, white);
                drawTextCentered(renderer, font_tiny, data_small, 150, white);
                display.drawString(0, 0, data_big.c_str());
                display.display();
                break;
            }

            case GuiCommand::DRAW_SPENDING: {
                // AFTER THE USER HAS SCANNED THEIR RFID CARD AND THE VALUE WAS = 0
                drawBackground(renderer, blue);
                std::string data_big;
                std::string data_small;
                {
                    std::lock_guard<std::mutex> lock(gui_data_mutex);
                    data_big = gui_data_big;
                    data_small = gui_data_small;
                }
                drawTextCentered(renderer, font_medium, data_big, 100, white);
                drawTextCentered(renderer, font_small, data_small, 150, white);
                display.drawString(0, 0, "Se skjerm");
                display.display();
                
                break;
            }

            case GuiCommand::DRAW_CHECKOUT: {
                drawBackground(renderer, green);
                std::string data_big;
                std::string data_small;
                {
                    std::lock_guard<std::mutex> lock(gui_data_mutex);
                    data_big = gui_data_big;
                    data_small = gui_data_small;
                }
                drawTextCentered(renderer, font_medium, data_big, 100, white);
                drawTextCentered(renderer, font_small, data_small, 150, white);
                display.drawString(0, 0, "Sum kritet");
                display.display();
                break;
            }

            case GuiCommand::DRAW_UNKOWN: {
                drawBackground(renderer, red);
                drawTextCentered(renderer, font_medium, "Ukjent kort!", 150, white);
                drawTextCentered(renderer, font_small, "Venligst kontakt Kasper :p", 260, white);
                display.drawString(0, 0, "Ukjent kort");
                display.display();
                break;
            }

            case GuiCommand::DRAW_END: {
                drawBackground(renderer, turk);
                drawTextCentered(renderer, font_medium, "Data lagret!", 150, white);
                drawTextCentered(renderer, font_small, "Kriteliste sendt til barsjef", 260, white);
                break;
            }

            default: break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(22);
    }

    // --- CLEANUP ---
    TTF_CloseFont(font_large);
    TTF_CloseFont(font_medium);
    TTF_CloseFont(font_small);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};