#ifndef GAMEBASE_HPP
#define GAMEBASE_HPP

class Renderer;

class GameBase {
public:
	virtual ~GameBase() {}

	virtual void update(float timeStep, float mouseX, float mouseY, bool pressed) = 0;
	virtual void render(Renderer& renderer) = 0;
};

#endif
