#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
int mx, my;
using namespace std;
const int screenWidth = 800, screenHeight = 600;

SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
SDL_Rect screen = { 0, 0, screenWidth, screenHeight};

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
    Player(LTexture* LT, SDL_Rect r) : PLTexture(LT), rect(r){}
    ~Player(){   }
    
    void walk(int dir, int x_step, int y_step){
        if(dir == 1) { rect.x += x_step; rect.y += y_step; }
    }
    
    void show(){ 
        if (PLTexture != NULL) {  PLTexture->renderScaled(&rect); }
    }
private:
    LTexture* PLTexture; 
    SDL_Rect rect;
};