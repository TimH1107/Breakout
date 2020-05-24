#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "vector2.hpp"

class Renderer;
class Game;

class Actor {

public:

	//Constructors, Destructors
	Actor(Game* inGame);
	Actor(Game* inGame, float inX, float inY, float inW, float inH);
	virtual ~Actor() {}

	//Standard functions
	virtual void update(float timeStep);
	virtual void render(Renderer& renderer);

	//GETTERS
	Vector2 getPosition() { return position; };
	Vector2 getCenter();
	float getWidth() { return w; };
	float getHeight() { return h; };
	Vector2 getVelocity() { return velocity; };
	virtual Vector2 getBoundingRectTopLeft();
	virtual Vector2 getBoundingRectBottomRight();

	//SETTERS
	void setPosition(const Vector2& v) { position = v; };
	void setWidth(float inW) { w = inW; };
	void setHeight(float inH) { h = inH; };
	virtual void addOffset(const Vector2& v);

	//Collision
	bool CheckCollision(Actor* const otherActor, float timeStep);
	bool CheckCollision(Actor* const otherActor, float timeStep, Vector2& HitPoint, Vector2& outNormal, float& outRatio);
	void SweepDeflect(float timeStep, float ratio, const Vector2& normal);
	
	//Lifecycle Management
	virtual void Destroy() { bKill = true; };
	bool IsPendingKill() { return bKill; };

protected:
	Game* game;

	Vector2 velocity;
	Vector2 position;
	float w;
	float h;

	bool bKill;
	
};

#endif