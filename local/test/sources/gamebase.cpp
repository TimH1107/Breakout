#include "gamebase.hpp"
#include "renderer.hpp"
#include "game.hpp"

#include <stdio.h>
#include <ctime>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

int main(  )
{
	srand(static_cast<int>(time(NULL)));

	SDL_Init(SDL_INIT_VIDEO);
	if (TTF_Init() < 0) printf("Error: %s \n", TTF_GetError());

	SDL_Window* pWindow = SDL_CreateWindow("Game",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			512, 512,
			0);
	SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_PRESENTVSYNC);
	Renderer renderer(pRenderer);

	TTF_Font* font = TTF_OpenFont("consolas.ttf", 20);
	renderer.SetFont(font);

	SDL_ShowCursor(SDL_DISABLE);

	Game* pGame = new Game();

	float mouseX = 256;
	float mouseY = 256;
	bool mousePressed = false;

	bool running = true;
	unsigned int lastTime = SDL_GetTicks();
	while(running) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
				break;
			case SDL_MOUSEMOTION:
				mouseX = ( float )event.motion.x;
				mouseY = ( float )event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mousePressed = true;
				break;
			case SDL_MOUSEBUTTONUP:
				mousePressed = false;
				break;
			}
		}

		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(pRenderer);

		unsigned int time = SDL_GetTicks();
		float timeStep = (time - lastTime) / 1000.0f;
		lastTime = time;
		pGame->update(timeStep, mouseX, mouseY, mousePressed);
		pGame->render(renderer);

		running = !pGame->IsGameOver();

		SDL_RenderPresent(pRenderer);
	}

	delete pGame;

	SDL_Quit();
	return 0;
}
