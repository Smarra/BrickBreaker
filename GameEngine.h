#pragma once

#include <Component/SimpleScene.h>

#include <vector>
#include <Core/Engine.h>
#include "Object_2D.h"

constexpr auto PI = 3.14159265;

class GameEngine : public SimpleScene
{
public:
	GameEngine();
	~GameEngine();

	void Init() override;
	const float SCREEN_HEIGHT = 720.0f;
	const float SCREEN_WIDTH = 1280.0f;

	const float WALL_WIDTH = 20.0f;

	const float BLOCK_GAP = 15.0f;
	const float BLOCK_HEIGHT = 30.0f;
	const float BLOCK_WIDTH = 60.0f;
	const float BLOCKS_PER_LINE = 12;
	const float BLOCKS_PER_COLUMN = 8;

	const float PLAYER_WIDTH = 140.0f;
	const float PLAYER_HEIGHT = 15.0f;

	const float BALL_RADIUS = 8.0f;
	const float BALL_GAP = 15.0f;
	const float BALL_SPEED = 230.0f;

	const float POWER_UP_SCALE = 3;
	const float POWER_UP_WIDTH = 20;
	const float SHIELD_OFFSET = 8;
	Object_2D *player, *ball, *powerUp, *pWall;
	int totalTime = 0, timeForPowerUp = 0;
	bool powerupReleased = false;
	int powerUpId = 0;
	int blocksHit = 0;
	int level = 0;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	bool GameEngine::CheckCollision(float posX, float posY, float width, float height);
	bool GameEngine::PowerUpCollision();
	float GameEngine::closestPoint(float posX, float posY, float width, float height);
	void GameEngine::resetGame();
	void GameEngine::initPowerUp();

protected:
	glm::mat3 modelMatrix;
	glm::mat3 playerMatrix;
	glm::mat3 ballMatrix;
	glm::mat3 powerUpMatrix;
	glm::mat3 blockMatrix;
	float translateX, translateY;
	float scaleX = 1, scaleY = 1;
	float angularStep;
	int mousePosX, mousePosY;			// The position of the mouse on the screen
	int startGame;						// Initially false, as the game is freezed
	int lives = 3;
	bool extraWall = false;
	
	// Objects 
	std::vector<Object_2D> listOfObstacles;
	std::vector<bool> hitBlocks;
	std::vector<float> scaleBlock;
	std::vector<Object_2D> blocks;
	std::vector<vector<Object_2D> > shields;
	std::vector<bool> hitShield;
};
