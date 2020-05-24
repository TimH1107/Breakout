#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "actor.hpp"
#include "vector2.hpp"
#include "renderer.hpp"

enum Renderer::Color;
class Renderer;
class Game;

class Block : public Actor {

public:
	enum class BlockType {
		NORMAL,
		EXTRA_LIFE,
		SHRINK,
		GROW
	};

	Block(Game* inGame);
	Block(Game* inGame, float inX, float inY, float inW, float inH, int inScore, Renderer::Color inColor);
	virtual ~Block() {}

	//make this block  a special item
	void makeSpecial();
	void makeSpecial(BlockType inBlockType);

	void update(float timeStep) override;
	void render(Renderer& renderer) override;

	int GetScore() { return score; };

	void addOffset(const Vector2& v) override;
	void Destroy() override;

protected:
	int score;
	Renderer::Color color;
	BlockType blockType;

};

#endif