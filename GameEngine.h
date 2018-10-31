#pragma once

#include <Component/SimpleScene.h>

#include <Core/Engine.h>

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
	const float BLOCK_HEIGHT = 25.0f;
	const float BLOCK_WIDTH = 50.0f;
	const float BLOCKS_PER_LINE = 12;
	const float BLOCKS_PER_COLUMN = 10;

	const float PLAYER_WIDTH = 140.0f;
	const float PLAYER_HEIGHT = 15.0f;
	const float OFFSET_PLAYER_WALL = 15.0f;

	const float BALL_RADIUS = 8.0f;
	const float BALL_GAP = 10.0f;
	const float BALL_SPEED = 180.0f;

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
	GLboolean GameEngine::CheckCollision(float posX, float posY, float width, float height);

protected:
	glm::mat3 modelMatrix;
	glm::mat3 playerMatrix;
	glm::mat3 ballMatrix;
	float translateX, translateY;
	float scaleX, scaleY;
	float angularStep;
	int mousePosX, mousePosY;			// The position of the mouse on the screen
	int startGame;						// Initially false, as the game is freezed
	float ballPosX, ballPosY;
	float velocityX, velocityY;
	
	// Objects positions
	std::vector <std::tuple<std::string, float, float> > listOfObstacles;
};
