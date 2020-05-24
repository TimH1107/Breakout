#ifndef GIFT_HPP
#define GIFT_HPP

#include "actor.hpp"
#include "vector2.hpp"

class Gift : public Actor {
public:
	Gift(Game* inGame);
	Gift(Game* inGame, const Vector2& location);

	void update(float timeStep) override;
	void render(Renderer& renderer) override;

	void Collect();
};

#endif