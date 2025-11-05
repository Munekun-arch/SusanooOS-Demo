#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH  800
#define HEIGHT 480

typedef struct {
    SDL_Rect rect;
    SDL_Color base, hover;
    const char *label;
} Button;

typedef struct {
    bool visible;
    float alpha;
    SDL_Rect rect;
    SDL_Color bg;
    const char *title;
} Window;

void fill_gradient(SDL_Renderer *ren, SDL_Color top, SDL_Color bottom) {
    for (int y = 0; y < HEIGHT; y++) {
        float t = (float)y / HEIGHT;
        Uint8 r = top.r * (1 - t) + bottom.r * t;
        Uint8 g = top.g * (1 - t) + bottom.g * t;
        Uint8 b = top.b * (1 - t) + bottom.b * t;
        SDL_SetRenderDrawColor(ren, r, g, b, 255);
        SDL_RenderDrawLine(ren, 0, y, WIDTH, y);
    }
}

void draw_button(SDL_Renderer *ren, TTF_Font *font, Button *btn, bool hover) {
    SDL_Color c = hover ? btn->hover : btn->base;
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, 180);
    SDL_RenderFillRect(ren, &btn->rect);

    SDL_Color textColor = {255,255,255,255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, btn->label, textColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
    SDL_Rect dst = {btn->rect.x + (btn->rect.w - tw)/2, btn->rect.y + (btn->rect.h - th)/2, tw, th};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void draw_window(SDL_Renderer *ren, TTF_Font *font, Window *w) {
    if (!w->visible) return;

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    // 影
    SDL_Rect shadow = {w->rect.x + 5, w->rect.y + 5, w->rect.w, w->rect.h};
    SDL_SetRenderDrawColor(ren, 0, 0, 0, (Uint8)(w->alpha * 100));
    SDL_RenderFillRect(ren, &shadow);

    // ウィンドウ本体
    SDL_SetRenderDrawColor(ren, w->bg.r, w->bg.g, w->bg.b, (Uint8)(w->alpha * 220));
    SDL_RenderFillRect(ren, &w->rect);

    // タイトルバー
    SDL_Rect titlebar = {w->rect.x, w->rect.y, w->rect.w, 28};
    SDL_SetRenderDrawColor(ren, 60, 90, 200, (Uint8)(w->alpha * 230));
    SDL_RenderFillRect(ren, &titlebar);

    // タイトルテキスト
    SDL_Color textColor = {255,255,255,255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, w->title, textColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
    SDL_Rect dst = {w->rect.x + 10, w->rect.y + 5, tw, th};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);

    // 閉じるボタン
    SDL_Rect close = {w->rect.x + w->rect.w - 30, w->rect.y + 4, 20, 20};
    SDL_SetRenderDrawColor(ren, 255, 80, 80, (Uint8)(w->alpha * 255));
    SDL_RenderFillRect(ren, &close);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("SusanooOS v5 - 光と影のGUI",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);

    Button btns[3] = {
        {{100, 380, 150, 50}, {70,100,200,255}, {100,150,255,255}, "Terminal"},
        {{325, 380, 150, 50}, {70,100,200,255}, {100,150,255,255}, "Editor"},
        {{550, 380, 150, 50}, {70,100,200,255}, {100,150,255,255}, "System"}
    };

    Window app = {false, 0.0f, {250,100,300,220}, {40,60,120,255}, "Susanoo App"};

    bool running = true;
    SDL_Event e;

    while (running) {
        int mx, my; SDL_GetMouseState(&mx, &my);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (app.visible) {
                    SDL_Rect close = {app.rect.x + app.rect.w - 30, app.rect.y + 4, 20, 20};
                    if (mx >= close.x && mx <= close.x+close.w && my >= close.y && my <= close.y+close.h)
                        app.visible = false;
                }
                for (int i=0; i<3; i++) {
                    if (mx >= btns[i].rect.x && mx <= btns[i].rect.x+btns[i].rect.w &&
                        my >= btns[i].rect.y && my <= btns[i].rect.y+btns[i].rect.h) {
                        app.visible = true; app.alpha = 0.0f; app.title = btns[i].label;
                    }
                }
            }
        }

        if (app.visible && app.alpha < 1.0f) app.alpha += 0.05f;

        // 背景グラデーション
        SDL_Color top = {10,15,40,255}, bottom = {25,35,80,255};
        fill_gradient(ren, top, bottom);

        // 発光ライン
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_ADD);
        for (int i=0; i<5; i++) {
            SDL_SetRenderDrawColor(ren, 80, 150, 255, 40);
            SDL_Rect glow = {100+i*30, 150+i*10, 600, 10};
            SDL_RenderFillRect(ren, &glow);
        }

        // ボタン
        for (int i=0; i<3; i++) {
            bool hover = (mx >= btns[i].rect.x && mx <= btns[i].rect.x+btns[i].rect.w &&
                          my >= btns[i].rect.y && my <= btns[i].rect.y+btns[i].rect.h);
            draw_button(ren, font, &btns[i], hover);
        }

        // ウィンドウ描画
        draw_window(ren, font, &app);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

