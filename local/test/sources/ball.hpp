#ifndef BALL_HPP
#define BALL_HPP

#include "actor.hpp"
#include "vector2.hpp"

class Renderer;
class Game;

class Ball : public Actor {

public:
	Ball(Game* inGame);
	virtual ~Ball() {}

	void update(float timeStep) override;
	void render(Renderer& renderer) override;

	void OnHit(Actor* otherActor, float timeStep, const Vector2&  HitLocation, const Vector2& HitNormal, float SweepRatio);


protected:
};

#endif