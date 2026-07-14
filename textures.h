
LTexture boardTexture;

LTexture textTexture;
LTexture emojiTexture;

SDL_Rect board = { 0, 0, boardWidth, boardHeight};
SDL_Rect smiley = { 60, 30, 150, 150};

enum State {
    Building = 1, 
    Setting ,
    Moving ,
    Rotating,
};

