#include <SDL2/SDL.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 480

typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color hoverColor;
    SDL_Color activeColor;
    bool active;
} Button;

void draw_button(SDL_Renderer *ren, Button *btn, bool hover) {
    if (btn->active) SDL_SetRenderDrawColor(ren, btn->activeColor.r, btn->activeColor.g, btn->activeColor.b, 255);
    else if (hover) SDL_SetRenderDrawColor(ren, btn->hoverColor.r, btn->hoverColor.g, btn->hoverColor.b, 255);
    else SDL_SetRenderDrawColor(ren, btn->color.r, btn->color.g, btn->color.b, 255);
    SDL_RenderFillRect(ren, &btn->rect);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SusanooOS GUI Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // 背景と光帯
    SDL_SetRenderDrawColor(ren, 20, 30, 60, 255);
    SDL_RenderClear(ren);
    SDL_SetRenderDrawColor(ren, 80, 130, 255, 255);
    SDL_Rect beam = {100, 140, 600, 160};
    SDL_RenderFillRect(ren, &beam);

    // ボタン群
    Button buttons[3] = {
        {{100, 380, 150, 50}, {60,90,200,255}, {90,130,255,255}, {120,160,255,255}, false},
        {{325, 380, 150, 50}, {60,90,200,255}, {90,130,255,255}, {120,160,255,255}, false},
        {{550, 380, 150, 50}, {60,90,200,255}, {90,130,255,255}, {120,160,255,255}, false},
    };

    bool running = true;
    SDL_Event e;
    while (running) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                for (int i = 0; i < 3; i++) {
                    if (mx >= buttons[i].rect.x && mx <= buttons[i].rect.x + buttons[i].rect.w &&
                        my >= buttons[i].rect.y && my <= buttons[i].rect.y + buttons[i].rect.h) {
                        buttons[i].active = !buttons[i].active;
                    }
                }
            }
        }

        // 再描画
        SDL_SetRenderDrawColor(ren, 20, 30, 60, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 80, 130, 255, 255);
        SDL_RenderFillRect(ren, &beam);

        for (int i = 0; i < 3; i++) {
            bool hover = (mx >= buttons[i].rect.x && mx <= buttons[i].rect.x + buttons[i].rect.w &&
                          my >= buttons[i].rect.y && my <= buttons[i].rect.y + buttons[i].rect.h);
            draw_button(ren, &buttons[i], hover);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

