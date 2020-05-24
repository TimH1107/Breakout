#ifndef RACKET_HPP
#define RACKET_HPP

#include "actor.hpp"

class Renderer;
class Game;

class Racket : public Actor {

public:
	Racket(Game* inGame);
	virtual ~Racket() {}

	void update(float timeStep) override;
	void render(Renderer& renderer) override;

protected:
};

#endif