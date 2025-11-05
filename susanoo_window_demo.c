#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define WIDTH 1024
#define HEIGHT 600

typedef struct {
    SDL_Rect rect;
    char title[64];
    bool dragging;
    int drag_offset_x, drag_offset_y;
} Window;

void draw_window(SDL_Renderer *ren, TTF_Font *font, Window *win) {
    // ウィンドウ背景
    SDL_SetRenderDrawColor(ren, 50, 70, 120, 255);
    SDL_RenderFillRect(ren, &win->rect);

    // タイトルバー
    SDL_Rect titlebar = {win->rect.x, win->rect.y, win->rect.w, 24};
    SDL_SetRenderDrawColor(ren, 80, 110, 180, 255);
    SDL_RenderFillRect(ren, &titlebar);

    // 枠線
    SDL_SetRenderDrawColor(ren, 200, 200, 255, 255);
    SDL_RenderDrawRect(ren, &win->rect);

    // タイトル文字
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, win->title, (SDL_Color){255, 255, 255, 255});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect dst = {win->rect.x + 8, win->rect.y + 4, surf->w, surf->h};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *win = SDL_CreateWindow("SusanooOS GUI Shell",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 16);

    Window windows[2] = {
        {{100, 100, 320, 240}, "ファイルビューア", false, 0, 0},
        {{460, 200, 300, 200}, "設定ウィンドウ", false, 0, 0}
    };

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x, my = e.button.y;
                for (int i = 0; i < 2; i++) {
                    SDL_Rect titlebar = {windows[i].rect.x, windows[i].rect.y, windows[i].rect.w, 24};
                    if (SDL_PointInRect(&(SDL_Point){mx, my}, &titlebar)) {
                        windows[i].dragging = true;
                        windows[i].drag_offset_x = mx - windows[i].rect.x;
                        windows[i].drag_offset_y = my - windows[i].rect.y;
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                for (int i = 0; i < 2; i++) windows[i].dragging = false;
            } else if (e.type == SDL_MOUSEMOTION) {
                int mx = e.motion.x, my = e.motion.y;
                for (int i = 0; i < 2; i++) {
                    if (windows[i].dragging) {
                        windows[i].rect.x = mx - windows[i].drag_offset_x;
                        windows[i].rect.y = my - windows[i].drag_offset_y;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 25, 35, 60, 255);
        SDL_RenderClear(ren);

        for (int i = 0; i < 2; i++) {
            draw_window(ren, font, &windows[i]);
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

