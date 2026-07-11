
LTexture boardTexture;
LTexture swordTexture;
LTexture textTexture;
LTexture emojiTexture;
SDL_Color textColor = { 255, 255, 255, 255 }; // White text
SDL_Rect board = { 0, 0, boardWidth, boardHeight};
SDL_Rect smiley = { 60, 30, 150, 150};

enum State {
    Setting = 1,
    Moving = 2,
    Rotating = 3
};

