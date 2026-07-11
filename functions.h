int getdir( SDL_Keysym K ){
    if (K.sym == SDLK_e) return 1;
    else if (K.sym == SDLK_d) return 2;
    else if (K.sym == SDLK_x) return 3;
    else if (K.sym == SDLK_z) return 4;
    else if (K.sym == SDLK_a) return 5;
    else if (K.sym == SDLK_w) return 6;
    else return 87;
}