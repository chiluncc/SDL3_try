#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace global {
	SDL_Window* window = nullptr;
	void error();
	void init(const int, const int, const Uint64);
	void quit();
}

void global::error() {
	std::string error = SDL_GetError();
	SDL_Log(error.c_str());
	SDL_Quit();
	exit(-1);
}

void global::init(const int width, const int height, const Uint64 flag) {
	global::window = SDL_CreateWindow("SDL3", width, height, flag);
	if (!global::window) global::error();
}

void global::quit() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* args) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "SDL init error" << std::endl;
		exit(-1);
	}
	global::init(800, 600, 0);
	bool quit = false;
	SDL_Event event;
	SDL_zero(event);
	while (!quit) {
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) quit = true;
		}
		SDL_Delay(50);
	}
	global::quit();
	return 0;
}