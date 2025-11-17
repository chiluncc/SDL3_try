#include <iostream>
#include <string>
#include <sstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

constexpr Uint64 WIDTH = 800;
constexpr Uint64 HEIGHT = 600;
constexpr Uint64 FPS = 60;

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

class TimeDisplay {
public:
	TimeDisplay() {
		font = TTF_OpenFont("fonts/ShinyCrystal.ttf", 64);
	}
	void quit() {
		TTF_CloseFont(font);
	}
	void display(SDL_Renderer* render, Uint64 curTime) {
		std::stringstream curTimeText;
		curTimeText << curTime;
		SDL_Surface* surface = TTF_RenderText_Blended(this->font, curTimeText.str().c_str(), 0, SDL_Color{ 255, 0, 0, 255 });
		SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
		SDL_FRect rect{ (WIDTH - surface->w) / 2, (HEIGHT - surface->h) / 2, surface->w, surface->h };
		SDL_RenderTexture(render, texture, nullptr, &rect);
		SDL_DestroySurface(surface);
		SDL_DestroyTexture(texture);
	}
private:
	TTF_Font* font;
};

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "SDL init error" << std::endl;
		exit(-1);
	}
	if (!TTF_Init()) SDL_Log("TTF init error");
	global::init(WIDTH, HEIGHT, NULL);
	
	bool quit = false;
	SDL_Event event;
	SDL_zero(event);

	TimeDisplay display;
	bool startTiming = false;
	Uint64 nextTime = 0, curTime = 0;
	while (!quit) {
		SDL_Delay(50);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) quit = true;
			else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				if (!startTiming)
					nextTime = SDL_GetTicks() + 1000;
				startTiming = true;
			}
		}
		SDL_RenderClear(global::render);
		if (startTiming && SDL_GetTicks() > nextTime) {
			curTime += 1000;
			nextTime = SDL_GetTicks() + 1000;
		}
		display.display(global::render, curTime / 1000);
		SDL_RenderPresent(global::render);
	}

	global::quit();
	return 0;
}