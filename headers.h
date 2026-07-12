#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
int mx, my, state = 1, steps;

char ch;
using namespace std;
const int boardWidth = 528, boardHeight = 475, screenWidth = 1000, screenHeight = 600, stepsMax = 20;

SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
SDL_Window* window = NULL;

int xcoord[40] = {
    0,
    150, 226, 302, 378,
    113, 189, 265, 340, 416,
    75, 150, 226, 302, 378, 454,
    37, 113, 189, 265, 340, 416, 492,
    75, 150, 226, 302, 378, 454,
    113, 189, 265, 340, 416,
    150, 226, 302, 378,
};

int ycoord[40] = {
    0,
    42, 42, 43, 43,
    107, 108, 108, 108, 109,
    172, 172, 172, 173, 173, 173,
    237, 237, 238, 238, 238, 239, 239,
    302, 302, 302, 303, 303, 303,
    367, 368, 368, 368, 369,
    432, 432, 433, 433,
};

int adja[40][7] = {
    {0, 0,  0,  0,  0,  0,  0}, // Index 0 (unused)

    // --- ROW 1 (1 to 4) ---
    {0, 0,  2,  6,  5,  0,  0},  // 1
    {0, 0,  3,  7,  6,  1,  0},  // 2
    {0, 0,  4,  8,  7,  2,  0},  // 3
    {0, 0,  0,  9,  8,  3,  0},  // 4

    // --- ROW 2 (5 to 9) ---
    {0, 1,  6,  11, 10, 0,  0},  // 5
    {0, 2,  7,  12, 11, 5,  1},  // 6
    {0, 3,  8,  13, 12, 6,  2},  // 7
    {0, 4,  9,  14, 13, 7,  3},  // 8
    {0, 0,  0,  15, 14, 8,  4},  // 9

    // --- ROW 3 (10 to 15) ---
    {0, 5,  11, 17, 16, 0,  0},  // 10
    {0, 6,  12, 18, 17, 10, 5},  // 11
    {0, 7,  13, 19, 18, 11, 6},  // 12
    {0, 8,  14, 20, 19, 12, 7},  // 13
    {0, 9,  15, 21, 20, 13, 8},  // 14
    {0, 0,  0,  22, 21, 14, 9},  // 15

    // --- ROW 4 / Center Row (16 to 22) ---
    {0, 10, 17, 23, 0 , 0,  0},  // 16
    {0, 11, 18, 24, 23, 16, 10}, // 17
    {0, 12, 19, 25, 24, 17, 11}, // 18
    {0, 13, 20, 26, 25, 18, 12}, // 19
    {0, 14, 21, 27, 26, 19, 13}, // 20
    {0, 15, 22, 28, 27, 20, 14}, // 21
    {0, 0,  0,  0,  28,  21, 15}, // 22

    // --- ROW 5 (23 to 28) ---
    {0, 17, 24, 29, 0 , 0,  16}, // 23
    {0, 18, 25, 30, 29, 23, 17}, // 24
    {0, 19, 26, 31, 30, 24, 18}, // 25
    {0, 20, 27, 32, 31, 26, 19}, // 26
    {0, 21, 28, 33, 32, 26, 20}, // 27
    {0, 22, 0,  0,  33,  27, 21}, // 28

    // --- ROW 6 (29 to 33) ---
    {0, 24, 30, 34, 0 , 0,  23}, // 29
    {0, 25, 31, 35, 34, 29, 24}, // 30
    {0, 26, 32, 36, 35, 30, 25}, // 31
    {0, 27, 33, 0,  36, 31, 26}, // 32
    {0, 28, 0,  0,  37, 32, 27}, // 33

    // --- ROW 7 (34 to 37) ---
    {0, 30, 35, 0,  0,  0,  29}, // 34
    {0, 31, 36, 0,  0,  34, 30}, // 35
    {0, 32, 37, 0,  0,  35, 31}, // 36
    {0, 33, 0,  0,  0,  36, 32}  // 37
};

class LTexture {
public:
    LTexture() {
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }

    ~LTexture() {
        free();
    }

    bool loadFromFile(string path, bool no_white = false) {
        free();

        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (!loadedSurface) {
            printf("Unable to load image %s! SDL_image Error: %s\n",
                   path.c_str(), IMG_GetError());
            return false;
        }

        if (no_white) {
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 255, 255));
        }

        mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (!mTexture) {
            printf("Unable to create texture from %s! SDL Error: %s\n",
                   path.c_str(), SDL_GetError());
            SDL_FreeSurface(loadedSurface);
            return false;
        }

        mWidth = loadedSurface->w;
        mHeight = loadedSurface->h;

        SDL_FreeSurface(loadedSurface);
        return true;
    }

    bool loadFromRenderedText(std::string textureText, SDL_Color textColor) {
        free();

        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(gFont, textureText.c_str(), textColor);

        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        mWidth = textSurface->w;
        mHeight = textSurface->h;

        SDL_FreeSurface(textSurface);
        return true;
    }

    void free() {
        if (mTexture) {
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
            mWidth = mHeight = 0;
        }
    }

    void setColor(Uint8 r, Uint8 g, Uint8 b) {
        if (mTexture) SDL_SetTextureColorMod(mTexture, r, g, b);
    }

    void setBlendMode(SDL_BlendMode blending) {
        if (mTexture) SDL_SetTextureBlendMode(mTexture, blending);
    }

    void setAlpha(Uint8 alpha) {
        if (mTexture) SDL_SetTextureAlphaMod(mTexture, alpha);
    }

    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        SDL_Rect renderQuad = { x, y, mWidth, mHeight };

        if (clip != NULL)
        {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
    }

    void renderScaled(SDL_Rect* Scaled = NULL, SDL_Rect* clip = NULL)
    {
        if (!mTexture || !Scaled) return;
        SDL_RenderCopy(gRenderer, mTexture, clip, Scaled);
    }

    void renderScaledRotated(SDL_Rect* dest, double angle = 0.0, SDL_Rect* clip = NULL, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE){
        if (!mTexture || !dest) return;
        SDL_RenderCopyEx(gRenderer, mTexture, clip, dest, angle, center, flip);
    }   //usage:swordTexture.renderScaledRotated(&rect, 45.0);
    int getWidth() { return mWidth; }
    int getHeight() { return mHeight; }

private:
    SDL_Texture* mTexture;
    int mWidth, mHeight;
};

class Player {
public:
    int sp, dp; //starting point, destination
    int sa, da; // starting angle, destination
    Player(LTexture* LT, SDL_Rect r, int Sp) : PLTexture(LT), sp(Sp){
        rect.x = xcoord[Sp] - rect.w / 2; rect.y = ycoord[Sp] - rect.h / 2;  
        rect.w = r.w; rect.h = r.h;  ang = 30; dirq.push_back(0);
        sa = 0;
    }
    ~Player(){   }
    
    void walk(int steps){
        rect.x = steps *(xcoord[dp]-xcoord[sp] )/stepsMax + xcoord[sp] - rect.w / 2; 
        rect.y = steps *(ycoord[dp]-ycoord[sp] )/stepsMax + ycoord[sp] - rect.h/2 ;      //steps is 1 to 100
    }
    void turn(int steps){
        ang = steps *(da-sa)/stepsMax + sa;
    }
    
    void show(){    if (PLTexture != NULL) {  PLTexture->renderScaledRotated(&rect, ang ); }  }
    void pushdir(int d){  dirq.push_back(d); }
    // returns 0 = nothing valid to do (queue empty or invalid move), 1 = move started, 2 = rotate started
    int set(){
        if (dirq.empty()) return 0;
        int d = dirq.front();
        dirq.pop_front();
        if (d >= 1 && d <= 6) {
            if (adja[sp][d] > 0) {
                dp = adja[sp][d];
                return 1;
            }
            return 0;   // invalid move — discarded, nothing happens this frame
        }
        else if (d >= 7 && d <= 12) {
            sa = ang;                      // fixes Bug 2 — start from current angle
            da = (d - 6) * 60 - 30;
            return 2;
        }
        return 0;
    }
    int getfront(){ if (dirq.empty()){ return 0;}
    else return dirq.front(); }
    void arrived(){ sp = dp;   }

private:
    LTexture* PLTexture; 
    SDL_Rect rect;
    int ang; // 1~6
    deque<int> dirq;
};