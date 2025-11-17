#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace global {
	SDL_Window* window = nullptr;
	SDL_Renderer* render = nullptr;
	void error();
	void init(const int, const int, const Uint64);
	void quit();
}

void global::error() {
	std::string error = SDL_GetError();
	SDL_Log(error.c_str());
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
	TTF_Quit();
	SDL_Quit();
}

SDL_Texture* loadImage(std::string);
SDL_Texture* loadFont(std::string);
int main(int argc, char* args) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "SDL init error" << std::endl;
		exit(-1);
	}
	if (!TTF_Init()) SDL_Log("TTF init error");
	global::init(800, 600, SDL_WINDOW_RESIZABLE);

	//auto image = loadImage("pics/p1.png");
	auto image = loadFont("hello");
	
	bool quit = false;
	SDL_Event event;
	SDL_zero(event);
	bool show = false;
	while (!quit) {
		SDL_Delay(50);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) quit = true;
			if (event.type == SDL_EVENT_KEY_UP) {
				if (event.key.key == SDLK_S) show = !show;
			}
		}
		SDL_RenderClear(global::render);
		if (show) {
			SDL_FRect area1 = SDL_FRect(0, 0, 400, 400);
			SDL_FRect area2 = SDL_FRect(200, 0, 400, 400);
			SDL_RenderTexture(global::render, image, nullptr, &area1);
			SDL_RenderTexture(global::render, image, nullptr, &area2);
		}
		SDL_RenderPresent(global::render);
	}
	SDL_DestroyTexture(image);
	image = nullptr;

	global::quit();
	return 0;
}

SDL_Texture* loadFont(std::string fonts) {
	TTF_Font* font = TTF_OpenFont("fonts/ShinyCrystal.ttf", 14);
	SDL_Surface* surface = TTF_RenderText_Blended(font, fonts.c_str(), 0, SDL_Color{255u, 128u, 255u, 255u});
	SDL_Texture* texture = SDL_CreateTextureFromSurface(global::render, surface);
	SDL_DestroySurface(surface);
	TTF_CloseFont(font);
	return texture;
}

SDL_Texture* loadImage(std::string path) {
	SDL_Surface* surface = nullptr;
	if (!(surface = IMG_Load(path.c_str()))) SDL_Log("Error Picture Path");
	//SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 0XC7, 0XCF, 0XDC));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(global::render, surface);
	SDL_DestroySurface(surface);
	//SDL_SetTextureColorMod(texture, 0u, 0u, 255u);
	SDL_SetTextureAlphaMod(texture, 64u);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	return texture;
}