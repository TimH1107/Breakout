#include "block.hpp"
#include "game.hpp"

#include <ctime>

//Default constructor
Block::Block(Game* inGame) : Actor(inGame)
{
	score = 0;
	blockType = BlockType::NORMAL;
	color = Renderer::MAGENTA;
}

//Parameterized constructor
Block::Block(Game* inGame, float inX, float inY, float inW, float inH, int inScore, Renderer::Color inColor) : Actor(inGame, inX, inY, inW, inH)
{
	score = inScore;
	color = inColor;
	blockType = BlockType::NORMAL;
}


//Make the block special. Randomly pick specialization
void Block::makeSpecial()
{
	int randomNumber = rand() % 3;
	switch (randomNumber) 
	{
	case 0:
		makeSpecial(BlockType::EXTRA_LIFE);
		break;
	case 1:
		makeSpecial(BlockType::GROW);
		break;
	case 2:
		makeSpecial(BlockType::SHRINK);
		break;
	}
}

//Make the block special, w/ specified specialization
void Block::makeSpecial(BlockType inBlockType)
{
	//save block type
	blockType = inBlockType;

	//save color, depending on block type
	switch (inBlockType) 
	{
	case BlockType::EXTRA_LIFE:
		color = Renderer::BLUE;
		break;
	case BlockType::GROW:
		color = Renderer::GREEN;
		break;
	case BlockType::SHRINK:
		color = Renderer::RED;
		break;
	}

	//special blocks don't contain score points
	score = 0;
}

void Block::update(float timeStep)
{
	__super::update(timeStep);
}

void Block::render(Renderer& renderer)
{
	__super::render(renderer);

	renderer.drawRect(position.x, position.y, w, h, color);
}

void Block::addOffset(const Vector2& v)
{
	__super::addOffset(v);

	//game over if a block touches the lower window bounds
	if (getBoundingRectBottomRight().y > 480.f) game->SetGameOver();
}

//DESTROY ROUTINE: triggers game events
void Block::Destroy()
{
	__super::Destroy();

	//Trigger game event, depending on block type
	switch (blockType) 
	{
	case BlockType::EXTRA_LIFE:
		game->SpawnGift(getCenter());
		break;
	case BlockType::GROW:
		game->GrowRacket();
		break;
	case BlockType::SHRINK:
		game->ShrinkRacket();
		break;
	default:
		game->AddScore(score);
		break;
	}
}
