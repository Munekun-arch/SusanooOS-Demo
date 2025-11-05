#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 480

typedef struct {
    SDL_Rect rect;
    SDL_Color color, hoverColor, activeColor;
    const char *label;
    bool active;
} Button;

typedef struct {
    bool visible;
    float alpha;
    SDL_Rect rect;
    SDL_Color bgColor;
    const char *title;
} Window;

void draw_button(SDL_Renderer *ren, TTF_Font *font, Button *btn, bool hover) {
    SDL_SetRenderDrawColor(ren,
        hover ? btn->hoverColor.r : btn->color.r,
        hover ? btn->hoverColor.g : btn->color.g,
        hover ? btn->hoverColor.b : btn->color.b,
        255);
    SDL_RenderFillRect(ren, &btn->rect);

    // ラベル描画
    SDL_Color textColor = {255,255,255,255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, btn->label, textColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
    SDL_Rect dst = {btn->rect.x + (btn->rect.w - tw)/2, btn->rect.y + (btn->rect.h - th)/2, tw, th};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void draw_window(SDL_Renderer *ren, TTF_Font *font, Window *win) {
    if (!win->visible) return;
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, win->bgColor.r, win->bgColor.g, win->bgColor.b, (Uint8)(win->alpha * 255));
    SDL_RenderFillRect(ren, &win->rect);

    // タイトル
    SDL_Color textColor = {255,255,255,255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, win->title, textColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
    SDL_Rect dst = {win->rect.x + 10, win->rect.y + 5, tw, th};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);

    // 閉じるボタン
    SDL_Rect close = {win->rect.x + win->rect.w - 25, win->rect.y + 5, 20, 20};
    SDL_SetRenderDrawColor(ren, 255, 100, 100, (Uint8)(win->alpha * 255));
    SDL_RenderFillRect(ren, &close);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *win = SDL_CreateWindow("SusanooOS GUI v4",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    if (!font) { printf("フォント読み込み失敗: %s\n", TTF_GetError()); return 1; }

    // ボタン
    Button buttons[3] = {
        {{100, 380, 150, 50}, {60,90,200,255}, {90,130,255,255}, {120,160,255,255}, "Terminal", false},
        {{325, 380, 150, 50}, {60,90,200,255}, {90,130,255,255}, {120,160,255,255}, "Editor", false},
        {{550, 380, 150, 50}, {60,90,200,255}, {90,130,255,255}, {120,160,255,255}, "System", false},
    };

    // ウィンドウ
    Window appWin = {false, 0.0f, {250, 100, 300, 200}, {40, 60, 120, 200}, "Susanoo App Window"};

    bool running = true;
    SDL_Event e;
    while (running) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (appWin.visible) {
                    SDL_Rect close = {appWin.rect.x + appWin.rect.w - 25, appWin.rect.y + 5, 20, 20};
                    if (mx >= close.x && mx <= close.x + close.w && my >= close.y && my <= close.y + close.h) {
                        appWin.visible = false;
                        appWin.alpha = 0.0f;
                    }
                }
                for (int i = 0; i < 3; i++) {
                    if (mx >= buttons[i].rect.x && mx <= buttons[i].rect.x + buttons[i].rect.w &&
                        my >= buttons[i].rect.y && my <= buttons[i].rect.y + buttons[i].rect.h) {
                        appWin.visible = true;
                        appWin.alpha = 0.0f;
                        appWin.title = buttons[i].label;
                    }
                }
            }
        }

        // アニメーション
        if (appWin.visible && appWin.alpha < 1.0f) appWin.alpha += 0.05f;

        // 背景＋光帯
        SDL_SetRenderDrawColor(ren, 20, 30, 60, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 80, 130, 255, 255);
        SDL_Rect beam = {100, 140, 600, 160};
        SDL_RenderFillRect(ren, &beam);

        // ボタン描画
        for (int i = 0; i < 3; i++) {
            bool hover = (mx >= buttons[i].rect.x && mx <= buttons[i].rect.x + buttons[i].rect.w &&
                          my >= buttons[i].rect.y && my <= buttons[i].rect.y + buttons[i].rect.h);
            draw_button(ren, font, &buttons[i], hover);
        }

        // ウィンドウ描画
        draw_window(ren, font, &appWin);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

