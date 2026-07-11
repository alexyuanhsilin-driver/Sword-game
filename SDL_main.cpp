#include "headers.h"
#include "textures.h"
#include "functions.h"

void start(){
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow("Swords", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

    gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    gFont = TTF_OpenFont("arialbd.ttf", 28);
    swordTexture.loadFromFile("pictures/sword_1.png");
    boardTexture.loadFromFile("pictures/Board.png");
    emojiTexture.loadFromFile("pictures/emojis.png");
}

int main(int argc, char* argv[]) {
    start();

    bool running = true;
    SDL_Event e;
    Uint32 startTime = 0;
    stringstream timeText;
    SDL_Rect playerBox = { 100, 100, 25, 90 }; 
    Player player1( &swordTexture, playerBox, 6);

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) running = false;

            if (e.type == SDL_KEYDOWN && getdir(e.key.keysym) <= 6){
                player1.setdir( getdir(e.key.keysym) );
                if( state == State::Setting ){
                    state = 2;
                    steps = 0;
                }
            }
        }
        if( state == State::Moving){
            player1.walk( steps ); 
            if( steps == stepsMax ) {state = 1; player1.arrived(); }
            else steps++;
        }
        SDL_GetMouseState(&mx, &my);

        SDL_SetRenderDrawColor(gRenderer, 135, 206, 235, 255);
        SDL_RenderClear(gRenderer);
        boardTexture.renderScaled(&screen);
        std::string mouseStr = "x = " + std::to_string(mx) + ", y = " + std::to_string(my) 
        + ", time = " + to_string( SDL_GetTicks() - startTime );
        textTexture.loadFromRenderedText(mouseStr, textColor);
        textTexture.render(50, 50);
        //emojiTexture.renderScaled( &smiley );
        
        SDL_Rect swords = { mx - 12, my - 45, 25, 90 };
        swordTexture.renderScaled(&swords);
        
         
        player1.show();       

        SDL_RenderPresent(gRenderer);
    }

{
    swordTexture.free();
    boardTexture.free();
    textTexture.free();
    
    TTF_CloseFont(gFont);
    gFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(window);
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
    return 0;
}