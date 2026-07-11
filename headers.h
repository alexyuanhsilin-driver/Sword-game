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
const int screenWidth = 800, screenHeight = 600, stepsMax = 20;

SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
SDL_Window* window = NULL;

// 1-based arrays containing the coordinates for all 37 hexagons
int xcoord[40] = {
    0, // Index 0 (unused)
    // Row 1 (4 hexagons)
    230, 345, 460, 575,
    // Row 2 (5 hexagons)
    173, 288, 403, 518, 633,
    // Row 3 (6 hexagons)
    115, 230, 345, 460, 575, 690,
    // Row 4 (7 hexagons)
    58,  173, 288, 403, 518, 633, 748,
    // Row 5 (6 hexagons)
    115, 230, 345, 460, 575, 690,
    // Row 6 (5 hexagons)
    173, 288, 403, 518, 633,
    // Row 7 (4 hexagons)
    230, 345, 460, 575
};

int ycoord[40] = {
    0, // Index 0 (unused)
    // Row 1
    50,  50,  50,  50,
    // Row 2
    133, 133, 133, 133, 133,
    // Row 3
    215, 215, 215, 215, 215, 215,
    // Row 4
    298, 298, 298, 298, 298, 298, 298,
    // Row 5
    380, 380, 380, 380, 380, 380,
    // Row 6
    463, 463, 463, 463, 463,
    // Row 7
    545, 545, 545, 545
};

// columns: 1=UpperRight, 2=Right, 3=LowerRight, 4=LowerLeft, 5=Left, 6=UpperLeft
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

    int getWidth() { return mWidth; }
    int getHeight() { return mHeight; }

private:
    SDL_Texture* mTexture;
    int mWidth, mHeight;
};

class Player {
public:
    int sp, dp; //starting point, destination
    Player(LTexture* LT, SDL_Rect r, int Sp) : PLTexture(LT), sp(Sp){
        rect.x = xcoord[Sp] - rect.w / 2; rect.y = ycoord[Sp] - rect.h/2;  
        rect.w = r.w; rect.h = r.h;
    }
    ~Player(){   }
    
    void walk(int steps){
        rect.x = steps *(xcoord[dp]-xcoord[sp] )/stepsMax + xcoord[sp] - rect.w / 2; 
        rect.y = steps *(ycoord[dp]-ycoord[sp] )/stepsMax + ycoord[sp] - rect.h/2 ;      //steps is 1 to 100
    }
    
    void show(){    if (PLTexture != NULL) {  PLTexture->renderScaled(&rect); }  }
    void setdir(int d){   dirq.push(d); if( adja[sp][dirq.front()] > 0) {
        dp = adja[sp][dirq.front()]; dirq.pop();  }}
    void arrived(){ sp = dp;  }

private:
    LTexture* PLTexture; 
    SDL_Rect rect;
    int dir; // 1~6
    queue<int> dirq;
};