#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

int main(int argc, char* args) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("init fail");
	}
	SDL_Log("init success");
	return 0;
}