#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

void Handle_Error() {
	std::string error = SDL_GetError();
	SDL_Log(error.c_str());
	SDL_Quit();
}

int main(int argc, char* args) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "SDL init error" << std::endl;
		exit(-1);
	}
	SDL_Window* window = nullptr;
	window = SDL_CreateWindow("hello SDL3", 800, 600, 0);
	if (!window) Handle_Error();
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	bool quit = false;
	SDL_Event event;
	SDL_zero(event);
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) quit = true;
		}
		SDL_Delay(8);
	}
	return 0;
}