#include <SDL2/SDL.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("SusanooOS Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(ren, 25, 35, 65, 255); // 背景（夜空色）
    SDL_RenderClear(ren);
    SDL_SetRenderDrawColor(ren, 80, 130, 255, 255); // 光の帯
    SDL_Rect rect = {100, 150, 600, 180};
    SDL_RenderFillRect(ren, &rect);
    SDL_RenderPresent(ren);
    SDL_Delay(2000);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
