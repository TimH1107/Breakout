#include "game.hpp"
#include "racket.hpp"
#include "renderer.hpp"

Racket::Racket(Game* inGame) : Actor(inGame)
{
	position.x = 0.f;
	position.y = 512.f - h;
	w = RACKET_BASE_WIDTH;
	h = 5.f;
}

void Racket::update(float timeStep)
{
	__super::update(timeStep);
}

void Racket::render(Renderer& renderer)
{
	__super::render(renderer);

	renderer.drawRect(position.x, 512.f - h, w, h, Renderer::DARK_GREEN);
}
