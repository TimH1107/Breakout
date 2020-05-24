#include "gift.hpp"
#include "game.hpp"
#include "renderer.hpp"

Gift::Gift(Game* inGame) : Actor(inGame)
{
}

Gift::Gift(Game* inGame, const Vector2& location) : Actor(inGame, location.x, location.y, 15.f, 15.f)
{
}

void Gift::update(float timeStep)
{
	//fall down with gravity
	velocity = velocity + Vector2(0.f, 200.f) * timeStep;
	position = position + velocity * timeStep;

	//self-destroy when the gift falls off the screen
	if (getBoundingRectTopLeft().y > 512.f) Destroy();
}

void Gift::render(Renderer& renderer)
{
	renderer.drawRect(position.x, position.y, w, h, Renderer::YELLOW);
	renderer.drawRect(position.x + 5.f, position.y, 5.f, h, Renderer::RED);
	renderer.drawRect(position.x, position.y + 5.f, w, 5.f, Renderer::RED);
}

//if gift has been collected by the player, award life through game event
void Gift::Collect()
{
	game->WonLife();
	Destroy();
}
