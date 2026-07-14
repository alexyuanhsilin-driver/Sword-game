#include "headers.h"
#include "textures.h"
#include "functions.h"

void start(){
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();

    window = SDL_CreateWindow("Swords", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

    gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    gFont = TTF_OpenFont("arialbd.ttf", 28);
    boardTexture.loadFromFile("pictures/Board5.png");
}

int main(int argc, char* argv[]) {
    start();

    bool running = true;
    SDL_Event e;
    Uint32 startTime = 0;
    stringstream timeText;
    SDL_Rect playerBox = { xcoord[6], ycoord[6], 80, 80 };     //change later
    Player player1( playerBox, 1);
    Queue queue1 (500, 100);

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) running = false;

            if (state == State::Building && e.type == SDL_KEYDOWN && getdir(e.key.keysym) <= 12){
                player1.pushdir( getdir(e.key.keysym) );
            }
            else if( state == State::Building && e.type == SDL_KEYDOWN && ( e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER )){  state = State::Setting;  }
        }
        if (state == State::Setting) {
            steps = 0;
            int t = player1.set(); 
            if (t == 0) state = State::Building;
            else if (t == 1) state = State::Moving;
            else if (t == 2) state = State::Rotating;// result == 0: invalid or empty queue — just stay in Setting, try again next frame
        }       
        if ( state == State::Moving){
            player1.walk( steps ); 
            if( steps == stepsMax ) {
                player1.arrived();   steps = 0;
                if( player1.getfront() == 0) state = State::Building; 
                else state = State::Setting;
            }
            else steps++;
        }
        else if( state == State::Rotating){
            player1.turn( steps ); 
            if( steps == stepsMax ) {
                steps = 0; player1.arrived(); 
                if( player1.getfront() == 0) state = State::Building; 
                else state = State::Setting;
                }
            else steps++;
        }
        SDL_GetMouseState(&mx, &my);
        SDL_RenderClear(gRenderer);
        SDL_SetRenderDrawColor(gRenderer, 80, 5, 80, 255);
        
        boardTexture.renderScaled(&board);
        std::string mouseStr = //"x = " + std::to_string(mx) + ", y = " + std::to_string(my) +
        //", time = " + to_string( SDL_GetTicks() - startTime ) + 
        " dirq.front = "+ to_string(player1.getfront()) + " state = "+ to_string(state);

        textTexture.loadFromRenderedText(mouseStr, textColor);
        textTexture.render(450, 10);
        queue1.show(player1);
        
         
        player1.show();       

        SDL_RenderPresent(gRenderer);
    }

{
    
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