#include "gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>

#define WINDOW_W 800
#define WINDOW_H 480
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"

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

void guiThread(std::atomic<bool>& run, std::atomic<GuiCommand>& gui_command) {
    // --- INIT ---
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "BarAqua",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font_large = TTF_OpenFont(FONT_PATH, 64);
    TTF_Font* font_medium = TTF_OpenFont(FONT_PATH, 36);
    TTF_Font* font_small = TTF_OpenFont(FONT_PATH, 24);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color black  = {0,   0,   0,   255};
    SDL_Color green  = {50,  200, 100, 255};
    SDL_Color red    = {200, 50,  50,  255};
    SDL_Color blue   = {50,  100, 200, 255};
    SDL_Color orange = {220, 140, 0,   255};

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
        }

        // --- DRAW ---
        switch (active_command) {

            case GuiCommand::DRAW_VALUE: {
                drawBackground(renderer, black);
                drawTextCentered(renderer, font_small, "Total spending", 100, white);
                // TODO: replace with actual value from user data
                drawTextCentered(renderer, font_large, "0 kr", 180, white);
                break;
            }

            case GuiCommand::DRAW_SPENDING: {
                drawBackground(renderer, green);
                drawTextCentered(renderer, font_medium, "Welcome!", 100, white);
                // TODO: replace with actual user name
                drawTextCentered(renderer, font_large, "Name", 180, white);
                drawTextCentered(renderer, font_small, "Balance: 0 kr", 300, white);
                break;
            }

            case GuiCommand::DRAW_CHECKOUT: {
                drawBackground(renderer, blue);
                drawTextCentered(renderer, font_medium, "Checking out", 100, white);
                // TODO: replace with actual user name and spending
                drawTextCentered(renderer, font_large, "Name", 180, white);
                drawTextCentered(renderer, font_small, "Total: 0 kr", 300, white);
                break;
            }

            case GuiCommand::DRAW_VALUE: {
                drawBackground(renderer, orange);
                drawTextCentered(renderer, font_medium, "Adding...", 100, white);
                // TODO: replace with actual value
                drawTextCentered(renderer, font_large, "+0 kr", 180, white);
                break;
            }

            case GuiCommand::DRAW_UNKOWN: {
                drawBackground(renderer, red);
                drawTextCentered(renderer, font_medium, "Unknown card!", 150, white);
                drawTextCentered(renderer, font_small, "Please register first", 260, white);
                break;
            }

            default: break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(22);  // ~60fps
    }

    // --- CLEANUP ---
    TTF_CloseFont(font_large);
    TTF_CloseFont(font_medium);
    TTF_CloseFont(font_small);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}