#ifndef GAME_HPP
#define GAME_HPP

#include "set"

#include "gamebase.hpp"
#include "actor.hpp"

const int RACKET_BASE_WIDTH = 100;
const int BLOCKS_PER_ROW = 14;
const int BLOCK_WIDTH = 33;
const int BLOCK_MARGIN_LEFT = 3;
const int BLOCK_MARGIN_INITIAL = 6;

class Game : public GameBase {
public:
	Game();
	virtual void update(float timeStep, float mouseX, float mouseY, bool pressed);
	virtual void render(Renderer& renderer);

private:

	enum GameStatus {
		NORMAL = 0,
		GROW = 1,	//racket is bigger for a short period of time
		SHRINK = 2	//racket is smaller for a short period of time
	};

	//Block and Row operations
	void SpawnRow();
	void ShiftDown(float offset);

	//Update
	void UpdateActors(float timeStep);
	void CheckCollisions(float timeStep);
	void UpdateGameStatus();
	bool CheckContinueGame(bool pressed);
	void UpdateLevelState();
	void UpdateMouse(float mouseX, float mouseY);

	//mouse coordinates
	float m_x, m_y;

	//main actors
	class Ball* ball;
	class Racket* racket;
	class Actor* wallTop;
	class Actor* wallLeft;
	class Actor* wallRight;

	//list of all actors
	std::set<Actor*> actors;

	//gameplay variables
	int progressionInterval;
	int spawnTimer;
	int growShrinkTimer;
	int score;
	int lives;
	int level;
	bool frozen;
	bool GameOver;
	GameStatus gameStatus;

public:
	void SetGameOver() { GameOver = true; }
	bool IsGameOver() { return GameOver; };
	void AddScore(int inScore);
	void WonLife();
	void LostLife();
	void GrowRacket();
	void ShrinkRacket();
	void SpawnGift(const Vector2& location);
};

#endif
