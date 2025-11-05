#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
    int type; // 0=none,1=terminal,2=editor,3=system
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

void draw_text(SDL_Renderer *ren, TTF_Font *font, int x, int y, const char *text, SDL_Color color) {
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void draw_button(SDL_Renderer *ren, TTF_Font *font, Button *btn, bool hover) {
    SDL_Color c = hover ? btn->hover : btn->base;
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, 180);
    SDL_RenderFillRect(ren, &btn->rect);
    draw_text(ren, font, btn->rect.x + 40, btn->rect.y + 15, btn->label, (SDL_Color){255,255,255,255});
}

void draw_terminal(SDL_Renderer *ren, TTF_Font *font, SDL_Rect rect, int frame) {
    SDL_SetRenderDrawColor(ren, 10, 10, 10, 230);
    SDL_RenderFillRect(ren, &rect);

    const char *lines[] = {
        "susanoo@core:~$ echo 'Hello, SusanooOS'",
        "Hello, SusanooOS",
        "susanoo@core:~$ uname -s",
        "Susanoo Kernel (FreeBSD-based)",
        "susanoo@core:~$ _"
    };

    SDL_Color txt = {0,255,0,255};
    int visible = frame / 60; // 1行/秒
    if (visible > 5) visible = 5;
    for (int i = 0; i < visible; i++) {
        draw_text(ren, font, rect.x + 10, rect.y + 40 + i * 28, lines[i], txt);
    }
}

void draw_window(SDL_Renderer *ren, TTF_Font *font, Window *w, int frame) {
    if (!w->visible) return;

    // 影と本体
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_Rect shadow = {w->rect.x + 4, w->rect.y + 4, w->rect.w, w->rect.h};
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 120);
    SDL_RenderFillRect(ren, &shadow);

    SDL_SetRenderDrawColor(ren, w->bg.r, w->bg.g, w->bg.b, (Uint8)(w->alpha * 220));
    SDL_RenderFillRect(ren, &w->rect);

    // タイトルバー
    SDL_Rect titlebar = {w->rect.x, w->rect.y, w->rect.w, 28};
    SDL_SetRenderDrawColor(ren, 60, 90, 200, (Uint8)(w->alpha * 255));
    SDL_RenderFillRect(ren, &titlebar);
    draw_text(ren, font, w->rect.x + 10, w->rect.y + 5, w->title, (SDL_Color){255,255,255,255});

    // 閉じるボタン
    SDL_Rect close = {w->rect.x + w->rect.w - 28, w->rect.y + 4, 20, 20};
    SDL_SetRenderDrawColor(ren, 255, 80, 80, 255);
    SDL_RenderFillRect(ren, &close);

    // 内容
    if (w->type == 1) draw_terminal(ren, font, w->rect, frame);
    else if (w->type == 2)
        draw_text(ren, font, w->rect.x + 20, w->rect.y + 60, "Text Editor (準備中)", (SDL_Color){200,200,255,255});
    else if (w->type == 3)
        draw_text(ren, font, w->rect.x + 20, w->rect.y + 60, "System Info (準備中)", (SDL_Color){200,255,200,255});
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *win = SDL_CreateWindow("SusanooOS v6 - Terminal Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 16);
    if (!font) { printf("Font load failed: %s\n", TTF_GetError()); return 1; }

    Button btns[3] = {
        {{100,380,150,50},{70,100,200,255},{100,150,255,255},"Terminal"},
        {{325,380,150,50},{70,100,200,255},{100,150,255,255},"Editor"},
        {{550,380,150,50},{70,100,200,255},{100,150,255,255},"System"}
    };

    Window app = {false, 0.0f, {220,80,360,300},{40,60,120,255},"App",0};

    bool running = true;
    int frame = 0;
    SDL_Event e;

    while (running) {
        int mx, my; SDL_GetMouseState(&mx,&my);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                SDL_Rect close = {app.rect.x + app.rect.w - 28, app.rect.y + 4, 20, 20};
                if (app.visible && mx >= close.x && mx <= close.x+close.w && my >= close.y && my <= close.y+close.h)
                    app.visible = false;

                for (int i=0;i<3;i++) {
                    if (mx >= btns[i].rect.x && mx <= btns[i].rect.x+btns[i].rect.w &&
                        my >= btns[i].rect.y && my <= btns[i].rect.y+btns[i].rect.h) {
                        app.visible = true;
                        app.alpha = 0.0f;
                        app.title = btns[i].label;
                        app.type = i+1;
                        frame = 0;
                    }
                }
            }
        }

        if (app.visible && app.alpha < 1.0f) app.alpha += 0.05f;

        SDL_Color top = {10,15,40,255}, bottom = {25,35,80,255};
        fill_gradient(ren, top, bottom);

        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_ADD);
        for (int i=0;i<5;i++){
            SDL_SetRenderDrawColor(ren, 80,150,255,40);
            SDL_Rect glow = {100+i*30,150+i*10,600,10};
            SDL_RenderFillRect(ren,&glow);
        }

        for (int i=0;i<3;i++){
            bool hover = (mx>=btns[i].rect.x && mx<=btns[i].rect.x+btns[i].rect.w &&
                          my>=btns[i].rect.y && my<=btns[i].rect.y+btns[i].rect.h);
            draw_button(ren, font, &btns[i], hover);
        }

        draw_window(ren, font, &app, frame);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
        frame++;
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

