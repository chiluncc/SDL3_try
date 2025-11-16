#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace global {
	SDL_Window* window = nullptr;
	SDL_Renderer* render = nullptr;
	void error();
	void error(std::string);
	void init(const int, const int, const Uint64);
	void quit();
}

void global::error() {
	std::string error = SDL_GetError();
	SDL_Log(error.c_str());
	SDL_Quit();
	exit(-1);
}

void global::error(std::string s) {
	SDL_Log(s.c_str());
	SDL_Quit();
	exit(-1);
}

void global::init(const int width, const int height, const Uint64 flag) {
	global::window = SDL_CreateWindow("SDL3", width, height, flag);
	global::render = SDL_CreateRenderer(window, nullptr);
	if (!global::window) global::error();
}

void global::quit() {
	SDL_DestroyRenderer(global::render);
	global::render = nullptr;
	SDL_DestroyWindow(global::window);
	global::window = nullptr;
	SDL_Quit();
}

SDL_Texture* loadImage(std::string);
int main(int argc, char* args) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "SDL init error" << std::endl;
		exit(-1);
	}
	global::init(800, 600, SDL_WINDOW_RESIZABLE);

	auto image = loadImage("C:\\Users\\lenovo\\Desktop\\Yumekui_Merry.jpg");
	
	bool quit = false;
	SDL_Event event;
	SDL_zero(event);
	while (!quit) {
		SDL_Delay(50);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) quit = true;
		}
		SDL_RenderClear(global::render);
		SDL_RenderTexture(global::render, image, NULL, NULL);
		SDL_RenderPresent(global::render);
	}
	SDL_DestroyTexture(image);
	image = nullptr;

	global::quit();
	return 0;
}

SDL_Texture* loadImage(std::string path) {
	SDL_Surface* surface = nullptr;
	if (!(surface = IMG_Load(path.c_str()))) global::error("Error Picture Path");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(global::render, surface);
	SDL_DestroySurface(surface);
	return texture;
}