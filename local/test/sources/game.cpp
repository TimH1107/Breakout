
#include <math.h>
#include <ctime>
#include <algorithm>

#include "game.hpp"
#include "renderer.hpp"
#include "vector2.hpp"

#include "ball.hpp"
#include "racket.hpp"
#include "block.hpp"
#include "gift.hpp"

Game::Game()
{

	//init gameplay variables
	score = 0;
	level = 0;
	lives = 3;
	frozen = true;
	GameOver = false;
	progressionInterval = 15; //time in seconds for adding new blocks / shifting blocks down
	gameStatus = GameStatus::NORMAL;

	//init mouse position
	m_x = 0.5f;
	m_y = 0.5f;

	//create the ball
	ball = new Ball(this);
	actors.insert(ball);

	//create the racket
	racket = new Racket(this);
	actors.insert(racket);

	//create the initial blocks
	for (int i=0; i<8; ++i)
	{
		++level;
		SpawnRow();
		ShiftDown(15.f);
	}
	ShiftDown(100.f);

	//create the (invisible) walls
	wallTop = new Actor(this, -100.f, -100.f, 512.f + 200.f, 100.f);
	actors.insert(wallTop);

	wallLeft = new Actor(this, -100.f, 0.f, 100.f, 512.f + 100.f);
	actors.insert(wallLeft);

	wallRight = new Actor(this, 512.f, 0.f, 100.f, 512.f + 100.f);
	actors.insert(wallRight);

	//initialize the timer, used for spawning new blocks
	spawnTimer = static_cast<int>(std::time(0));
}

//UPDATE ROUTINE
void Game::update(float timeStep, float mouseX, float mouseY, bool pressed)
{
	UpdateGameStatus();
	if (!CheckContinueGame(pressed)) return;
	UpdateMouse(mouseX, mouseY);
	UpdateLevelState();
	UpdateActors(timeStep);
	CheckCollisions(timeStep);
}

void Game::render(Renderer& renderer)
{
	//render all actors
	for (auto ItActor : actors)
	{
		Actor* actor = ItActor;
		if (actor == nullptr) continue;
		actor->render(renderer);
	}

	//render HUD
	renderer.drawText("Score: " + std::to_string(score), 0, 0, Renderer::WHITE);
	renderer.drawText("Lives: " + std::to_string(lives), 120, 0, Renderer::WHITE);
	if (frozen) renderer.drawText("Click to start", 320, 0, Renderer::WHITE);
}

//UTIL FUNCTION for spawning an entire row of blocks
void Game::SpawnRow()
{
	Renderer::Color color;

	//first two rows give 1 point per block, rows 3+4 give 3 points per block, then 5, 7, 9, ...
	int thisScore = 1 + ((level - 1) / 2) * 2;

	//some colorful blocks
	switch (((level - 1) / 2) % 10)
	{
	case 0:
		color = Renderer::DARK_YELLOW;
		break;
	case 1:
		color = Renderer::MAGENTA;
		break;
	case 2:
		color = Renderer::DARK_GREEN;
		break;
	case 3:
		color = Renderer::DARK_CYAN;
		break;
	case 4:
		color = Renderer::DARK_RED;
		break;
	case 5:
		color = Renderer::YELLOW;
		break;
	case 6:
		color = Renderer::DARK_YELLOW;
		break;
	case 7:
		color = Renderer::DARK_BLUE;
		break;
	case 8:
		color = Renderer::DARK_GREEN;
		break;
	case 9:
		color = Renderer::DARK_RED;
		break;
	}

	//spawn the individual block actors
	for (int i = 0; i < BLOCKS_PER_ROW; ++i)
	{
		//create block
		Block* newBlock = new Block(this, (float)(BLOCK_MARGIN_INITIAL + (BLOCK_MARGIN_LEFT + BLOCK_WIDTH) * i), 25.f, (float)BLOCK_WIDTH, 10.f, thisScore, color);

		//Make block special with a chance of 1:15
		if (rand() % 15 == 0) newBlock->makeSpecial();

		//add block to actors list
		actors.insert(newBlock);
	}
}

//Shift all block actors down by the given offset
void Game::ShiftDown(float offset)
{
	//Iterate through all actors
	std::set<Actor*>::iterator ItActor;
	ItActor = actors.begin();
	while (ItActor != actors.end())
	{

		//filter for block actors
		Block* block = dynamic_cast<Block*>(*ItActor);
		if (block)
		{
			block->addOffset(Vector2(0.f, offset));
		}

		++ItActor;
	}
}

//UPDATE ALL ACTORS
void Game::UpdateActors(float timeStep)
{
	std::set<Actor*>::iterator ItActor;
	ItActor = actors.begin();
	while (ItActor != actors.end()) {

		Actor* actor = *ItActor;
		if (actor != nullptr)
		{

			//Destroy actors with kill pending
			if (actor->IsPendingKill())
			{
				ItActor = actors.erase(ItActor);
				continue;
			}

			//Position the racket
			Racket* racket = dynamic_cast<Racket*>(*ItActor);
			if (racket)
			{
				racket->setPosition(Vector2(m_x - racket->getWidth() / 2, racket->getBoundingRectTopLeft().y));
			}

			//Update actor
			actor->update(timeStep);
		}
		++ItActor;
	}
}

//CHECK COLLISION
void Game::CheckCollisions(float timeStep)
{
	std::set<Actor*>::iterator ItActor;
	ItActor = actors.begin();
	while (ItActor != actors.end())
	{
		Actor* actor = *ItActor;
		if (actor != nullptr)
		{
			//out parameters for collision checks
			Vector2 HitPoint;
			Vector2 HitNormal;
			float SweepingRatio;

			//Do collision checks for the ball
			if (actor != ball)
			{
				if (ball->CheckCollision(actor, timeStep, HitPoint, HitNormal, SweepingRatio))
				{
					ball->OnHit(actor, timeStep, HitPoint, HitNormal, SweepingRatio);
				}
			}

			//Do collision checks for the racket and the gifts
			if (actor != racket)
			{
				Gift* gift = dynamic_cast<Gift*>(actor);
				if (gift != nullptr)
				{
					if (gift->CheckCollision(racket, timeStep, HitPoint, HitNormal, SweepingRatio))
					{
						gift->Collect();
					}
				}
			}

		}
		++ItActor;
	}
}

//Updates the game status
void Game::UpdateGameStatus()
{
	//if the racket is currently bigger/smaller than usual and if this status has persisted for 15 seconds, return to normal game state

	if (gameStatus != GameStatus::NORMAL && static_cast<int>(std::time(0)) - growShrinkTimer > 15)
	{
		gameStatus = GameStatus::NORMAL;
		racket->setWidth(RACKET_BASE_WIDTH);
	}
}

//Checks whether we should continue the game after we lost a life
bool Game::CheckContinueGame(bool pressed)
{
	if (frozen)
	{
		//If MouseClick
		if (pressed)
		{
			//unfreeze
			frozen = false;

			if (lives > 0)
			{
				//we still got lives, reset the ball and continue
				ball->setPosition(Vector2(256.f, 300.f));
			}
			else
			{
				//we're out of lives, terminate program
				GameOver = true;
				return false;
			}
		}
		else
		{
			//no mouse click, stay frozen
			return false;
		}
	}

	return true;
}

//periodically adds new blocks
void Game::UpdateLevelState()
{
	//Add new blocks
	if (std::time(0) - spawnTimer > progressionInterval)
	{
		++level;
		ShiftDown(15.f);
		if ((level - 5) % 8 < 4) SpawnRow();
		spawnTimer = static_cast<int>(std::time(0));
	}
}

//updates the mouse input
void Game::UpdateMouse(float mouseX, float mouseY)
{
	m_x = std::max(std::min(mouseX, 512.f - racket->getWidth() / 2), racket->getWidth() / 2);
	m_y = mouseY;
}

//Adds points to the total game score
void Game::AddScore(int inScore)
{
	score += inScore;
}

void Game::WonLife()
{
	++lives;
}

void Game::LostLife()
{
	--lives;
	frozen = true;
}

void Game::GrowRacket()
{
	gameStatus = GameStatus::GROW;
	racket->setWidth(RACKET_BASE_WIDTH * 2);
	growShrinkTimer = static_cast<int>(std::time(0));
}

void Game::ShrinkRacket()
{
	gameStatus = GameStatus::SHRINK;
	racket->setWidth((float)RACKET_BASE_WIDTH / 2.f);
	growShrinkTimer = static_cast<int>(std::time(0));
}

void Game::SpawnGift(const Vector2& location)
{
	Gift* gift = new Gift(this, location);
	actors.insert(gift);
}
