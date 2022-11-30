#include <iostream>
#include <vector>
#include <random>

#include <SDL2/include/SDL2/SDL.h>

#define DOT_SIZE 4
#define SCREEN_WIDTH (DOT_SIZE * 120) // each frame is 120 pixels wide
#define SCREEN_HEIGHT (DOT_SIZE * 200) // each frame is 200 pixels tall

// random boolean generator for generating dots
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> b_dist(0, 1);
bool randBool(){
    return b_dist(mt);
}

// dots shifting for displaying rectangles
void shiftRect(std::vector<std::vector<bool>>& dots, SDL_Rect shift_rect, const int SHIFT_BY = 2){
    for (int y = shift_rect.y; y != shift_rect.y + shift_rect.h; y++){
        for (int x = shift_rect.x; x != shift_rect.x + shift_rect.w; x++){

            if (x >= dots[0].size() || x < 0 || y >= dots.size() || y < 0) continue;

            int nx = x;
            bool dot = dots[y][nx];

            for (int i = 0; i != SHIFT_BY; i++){
                nx -= 1; // shift dots to left
                if (nx < 0 || nx >= dots[y].size()) continue; // out of range, dont do anything
                dots[y][nx] = dot;
            }
            for (int offset = 1; offset != SHIFT_BY + 1; offset++){
                dots[y][nx + offset] = randBool();
            }
        }
    }
}

int main(int argc, char* argv[]){

    // sdl2
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* win = SDL_CreateWindow("rd stereogram", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * 2, SCREEN_HEIGHT, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);

    // dots matrix
    std::vector<std::vector<bool>> dots(SCREEN_HEIGHT / DOT_SIZE, std::vector<bool>(SCREEN_WIDTH / DOT_SIZE));

    // frame limiting
    const int FPS = 30;
    const float FRAME_DELAY = 1000 / FPS;
    Uint32 frame_start = 0;
    int frame_time = 0;

    // mainloop
    bool win_quit = false;
    while (!win_quit){

        // event handling
        SDL_Event ev;
        while (SDL_PollEvent(&ev) != 0){
            if (ev.type == SDL_QUIT){
                win_quit = true;
            }
        }

        // render display clear
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 225, 225, 225, 255);

        // generate dots for both sides and show unedited (left side)
        for (int y = 0; y != SCREEN_HEIGHT / DOT_SIZE; y++){
            for (int x = 0; x != SCREEN_WIDTH / DOT_SIZE; x++){
                bool rand = randBool();
                dots[y][x] = rand;
                if (rand){
                    SDL_Rect dot_rct = {x * DOT_SIZE, y * DOT_SIZE, DOT_SIZE, DOT_SIZE};
                    SDL_RenderFillRect(ren, &dot_rct);
                }
            }
        }

        // testing: draw a boundary
        shiftRect(dots, {0, 0, 10, 200});
        shiftRect(dots, {120 - 10, 0, 10, 200});
        shiftRect(dots, {0, 0, 120, 10});
        shiftRect(dots, {0, 200 - 10, 120, 10});

        // now show the edited dots
        for (int y = 0; y != dots.size(); y++){
            for (int x = 0; x != dots.size(); x++){
                if (dots[y][x]){
                    SDL_Rect dot_rct = {SCREEN_WIDTH + x * DOT_SIZE, y * DOT_SIZE, DOT_SIZE, DOT_SIZE};
                    SDL_RenderFillRect(ren, &dot_rct);
                }
            }
        }
        SDL_RenderPresent(ren);

        // frame track
        frame_time = SDL_GetTicks() - frame_start;
        if (FRAME_DELAY > frame_time){
            SDL_Delay(FRAME_DELAY - frame_time);
        }
        std::cout << (FRAME_DELAY - frame_time) << "ms\n";
        frame_start = SDL_GetTicks();

    }

    // cleanup
    SDL_Quit();
}