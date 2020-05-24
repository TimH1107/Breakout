#include "game.hpp"
#include "actor.hpp"

#include <algorithm>
#include <limits>

//Default Constructor
Actor::Actor(Game* inGame)
{
	game = inGame;
	position.x = 0.f;
	position.y = 0.f;
	velocity.x = 0.f;
	velocity.y = 0.f;
	w = 0.f;
	h = 0.f;
	bKill = false;
}

//Parameterized Constructor
Actor::Actor(Game* inGame, float inX, float inY, float inW, float inH)
{
	game = inGame;
	position.x = inX;
	position.y = inY;
	velocity.x = 0.f;
	velocity.y = 0.f;
	w = inW;
	h = inH;
	bKill = false;
}

void Actor::update(float timeStep)
{
	//default behavior can be added here
}

void Actor::render(Renderer& renderer)
{
	//default behavior can be added here
}

//Returns the center of the actor
Vector2 Actor::getCenter()
{
	return Vector2(position.x + w / 2, position.y + h / 2);
}

//Returns the top left corner of the collision box
Vector2 Actor::getBoundingRectTopLeft()
{
	return position;
}

//Returns the bottom right corner of the collision box
Vector2 Actor::getBoundingRectBottomRight()
{
	return Vector2(position.x + w, position.y + h);
}

//Simple collision check, returns true (colliding) or false (not colliding)
bool Actor::CheckCollision(Actor* const otherActor, float timeStep)
{
	float ignore;
	return CheckCollision(otherActor, timeStep, Vector2(), Vector2(), ignore);
}

//Full collision check, returns true/false plus location of hit, normal of hit surface, sweeping ratio
bool Actor::CheckCollision(Actor* const otherActor, float timeStep, Vector2& HitPoint, Vector2& outNormal, float& outRatio)
{

	//scale the velocity according to the current timeStep
	Vector2 scaledVelocity = velocity * timeStep;

	//Quick proximity check, saves performance
	if ((getCenter() - otherActor->getCenter()).length() > (scaledVelocity.length() + std::max(w, h)/2 + std::max(otherActor->getWidth(), otherActor->getHeight()) / 2) * 1.1f) return false;

	//Swept AABB collision adapted from https://www.gamedev.net/articles/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/
	float xInvEntry, yInvEntry;
	float xInvExit, yInvExit;

	if (scaledVelocity.x > 0.0f)
	{
		xInvEntry = otherActor->getBoundingRectTopLeft().x - getBoundingRectBottomRight().x;
		xInvExit = otherActor->getBoundingRectBottomRight().x - getBoundingRectTopLeft().x;
	}
	else
	{
		xInvEntry = otherActor->getBoundingRectBottomRight().x - getBoundingRectTopLeft().x;
		xInvExit = otherActor->getBoundingRectTopLeft().x - getBoundingRectBottomRight().x;
	}

	if (scaledVelocity.y > 0.0f)
	{
		yInvEntry = otherActor->getBoundingRectTopLeft().y - getBoundingRectBottomRight().y;
		yInvExit = otherActor->getBoundingRectBottomRight().y - getBoundingRectTopLeft().y;
	}
	else
	{
		yInvEntry = otherActor->getBoundingRectBottomRight().y - getBoundingRectTopLeft().y;
		yInvExit = otherActor->getBoundingRectTopLeft().y - getBoundingRectBottomRight().y;
	}

	float xEntry, yEntry;
	float xExit, yExit;

	if (scaledVelocity.x == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xInvEntry / scaledVelocity.x;
		xExit = xInvExit / scaledVelocity.x;
	}

	if (scaledVelocity.y == 0.0f)
	{
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yInvEntry / scaledVelocity.y;
		yExit = yInvExit / scaledVelocity.y;
	}

	float entryTime = std::max(xEntry, yEntry);
	float exitTime = std::min(xExit, yExit);

	//check if collision has occured
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f) return false;

	//save the sweeping ratio (0 < r < 1) example: collision occured 60% into the last frame: r=0.6
	outRatio = entryTime;

	//save the collision normal. useful for deflection
	if (xEntry > yEntry)
	{
		if (xInvEntry < 0.0f)
		{
			outNormal = Vector2(1.f, 0.f);
		}
		else
		{
			outNormal = Vector2(-1.f, 0.f);
		}
	}
	else
	{
		if (yInvEntry < 0.0f)
		{
			outNormal = Vector2(0.f, 1.f);
		}
		else
		{
			outNormal = Vector2(0.f, -1.f);
		}
	}

	//save the hit point, useful for determining the pricese point of impact on the racket
	HitPoint.x = position.x - velocity.x * timeStep * (1.f - entryTime);
	HitPoint.y = position.y - velocity.y * timeStep * (1.f - entryTime);

	return true;
}

//Deflect ball considering sweeping
void Actor::SweepDeflect(float timeStep, float ratio, const Vector2& normal)
{
	//Go back to the point of collision
	position.x = position.x - velocity.x * timeStep * (1.f - ratio);
	position.y = position.y - velocity.y * timeStep * (1.f - ratio);

	//Reflect velocity according to collision normal
	velocity = velocity.reflect(normal);

	//Finish the frame according to the new velocity
	position.x += velocity.x * timeStep * (1.f - ratio);
	position.y += velocity.y * timeStep * (1.f - ratio);
}

//Adds an (x|y) to the current position
void Actor::addOffset(const Vector2& v)
{
	position = position + v;
}
