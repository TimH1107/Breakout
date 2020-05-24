#include "ball.hpp"

#include <math.h>

#include "renderer.hpp"
#include "game.hpp"

#include "racket.hpp"
#include "block.hpp"

//Default constructor
Ball::Ball(Game* inGame) : Actor(inGame)
{
	position.x = 256.f;
	position.y = 300.f;
	velocity.x = 300.f;
	velocity.y = 150.f;
	w = 5.f;
	h = 5.f;
}

//Update routine
void Ball::update(float timeStep)
{
	__super::update(timeStep);

	//linear movement
	position = position + velocity * timeStep;

	//check if the ball has fallen off the screen
	if (getBoundingRectBottomRight().y > 512.f) game->LostLife();
}

//Render routine
void Ball::render(Renderer& renderer)	
{
	__super::render(renderer);

	renderer.drawRect(position.x, position.y, w, h, Renderer::DARK_GREEN);
}

//Collision routine, called by the main loop in Game class
void Ball::OnHit(Actor* otherActor, float timeStep, const Vector2& HitLocation, const Vector2& HitNormal, float SweepRatio)
{

	//if the other actor is a racket...
	Racket* racket = dynamic_cast<Racket*>(otherActor);
	if (racket != nullptr)
	{
		Vector2 NewNormal = HitNormal;

		//deflect ball depending on the point of impact and the direction of the ball movement
		if (getPosition().x >= racket->getBoundingRectTopLeft().x && getPosition().x <= racket->getBoundingRectTopLeft().x + racket->getWidth() / 2)
		{

			//left side of racket
			NewNormal = getVelocity().x >= 0 ? Vector2(-1.f, -1.f) : Vector2(0.f, -1.f);
		}
		else
		{

			//right side of racket
			NewNormal = getVelocity().x >= 0 ? Vector2(0.f, -1.f) : Vector2(1.f, -1.f);
		}

		SweepDeflect(timeStep, SweepRatio, NewNormal);

		return;
	}

	//if the other actor is a block...
	Block* block = dynamic_cast<Block*>(otherActor);
	if (block != nullptr)
	{

		SweepDeflect(timeStep, SweepRatio, HitNormal);

		//destroy block. the block will handle events such as adding score
		block->Destroy();

		return;

	}

	//for all other actors
	SweepDeflect(timeStep, SweepRatio, HitNormal);
}