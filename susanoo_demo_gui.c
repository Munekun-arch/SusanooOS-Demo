#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 480

typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    const char *label;
    bool hovered;
} Button;

void draw_button(SDL_Renderer *ren, TTF_Font *font, Button btn) {
    SDL_SetRenderDrawColor(ren,
        btn.hovered ? btn.color.r + 40 : btn.color.r,
        btn.hovered ? btn.color.g + 40 : btn.color.g,
        btn.hovered ? btn.color.b + 40 : btn.color.b,
        255);
    SDL_RenderFillRect(ren, &btn.rect);
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderDrawRect(ren, &btn.rect);

    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, btn.label, (SDL_Color){255, 255, 255, 255});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    int tw = surf->w, th = surf->h;
    SDL_FreeSurface(surf);
    SDL_Rect dst = { btn.rect.x + (btn.rect.w - tw)/2, btn.rect.y + (btn.rect.h - th)/2, tw, th };
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window *win = SDL_CreateWindow("SusanooOS GUI Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 20);
    if (!font) {
        printf("フォントを読み込めません: %s\n", TTF_GetError());
        return 1;
    }

    Button buttons[3] = {
        {{100, 150, 200, 80}, {90, 140, 255, 255}, "起動", false},
        {{300, 150, 200, 80}, {90, 200, 90, 255}, "設定", false},
        {{500, 150, 200, 80}, {200, 80, 80, 255}, "終了", false}
    };

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            else if (e.type == SDL_MOUSEMOTION) {
                int mx = e.motion.x, my = e.motion.y;
                for (int i = 0; i < 3; i++) {
                    buttons[i].hovered = SDL_PointInRect(&(SDL_Point){mx, my}, &buttons[i].rect);
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                for (int i = 0; i < 3; i++) {
                    if (SDL_PointInRect(&(SDL_Point){mx, my}, &buttons[i].rect)) {
                        if (i == 0) printf("[Susanoo] 起動ボタンが押されました。\n");
                        if (i == 1) printf("[Susanoo] 設定ボタンが押されました。\n");
                        if (i == 2) running = false;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 25, 35, 65, 255);
        SDL_RenderClear(ren);

        for (int i = 0; i < 3; i++) {
            draw_button(ren, font, buttons[i]);
        }

        SDL_RenderPresent(ren);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

