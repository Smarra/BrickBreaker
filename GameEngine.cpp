#include "GameEngine.h"

#include <iostream>

#include <Core/Engine.h>
#include "Transformation2D.h"
#include "CreateObject2D.h"

using namespace std;

GameEngine::GameEngine()
{
}

GameEngine::~GameEngine()
{
}

void GameEngine::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	// start game
	Object_2D *obj;
	startGame = 0;
	for (int i = 0; i < BLOCKS_PER_COLUMN * BLOCKS_PER_LINE; i++)
		hitBlocks.push_back(false);

	// Blocks
	float ys = SCREEN_HEIGHT - 100.0f;
	float xs = (SCREEN_WIDTH - BLOCKS_PER_LINE * (BLOCK_WIDTH + BLOCK_GAP) + BLOCK_GAP) / 2;
	for (int i = 0; i < BLOCKS_PER_COLUMN; i++) {
		for (int j = 0; j < BLOCKS_PER_LINE; j++) {
			int index = i * BLOCKS_PER_LINE + j;
			std::string name("Block " + to_string(index));
			obj = new Object_2D(name, xs, ys, BLOCK_WIDTH, BLOCK_HEIGHT, 0, 0, glm::vec3(1, 0, 0), false);
			listOfObstacles.push_back(*obj);
			xs += BLOCK_GAP + BLOCK_WIDTH;
		}
		ys -= BLOCK_GAP + BLOCK_HEIGHT;
		xs = (SCREEN_WIDTH - BLOCKS_PER_LINE * (BLOCK_WIDTH + BLOCK_GAP) + BLOCK_GAP) / 2;
	}
	
	// Walls
	float off = 2 * BALL_GAP + PLAYER_HEIGHT;
	obj = new Object_2D("Wall Left", 0, off, WALL_WIDTH, SCREEN_HEIGHT - off, 0, 0, glm::vec3(1, 0, 0), false);
	listOfObstacles.push_back(*obj);
	obj = new Object_2D("Wall Up", 0, SCREEN_HEIGHT - WALL_WIDTH, SCREEN_WIDTH, WALL_WIDTH, 0, 0, glm::vec3(1, 0, 0), false);
	listOfObstacles.push_back(*obj);
	obj = new Object_2D("Wall Right", SCREEN_WIDTH - WALL_WIDTH, off, WALL_WIDTH, SCREEN_HEIGHT - off, 0, 0, glm::vec3(1, 0, 0), false);
	listOfObstacles.push_back(*obj);

	// Player
	player = new Object_2D("Player", 0, BALL_GAP, PLAYER_WIDTH, PLAYER_HEIGHT, 0, 0, glm::vec3(1, 0, 1), false);

	// Balls
	off = BALL_RADIUS + BALL_GAP;
	ball = new Object_2D("Ball 0", 0, 0, BALL_RADIUS, -40 + rand() % 80, BALL_SPEED, glm::vec3(1, 1, 0), true);

	obj = new Object_2D("Ball 1", off, off, BALL_RADIUS, 0, 0, glm::vec3(1, 0, 0), true);
	listOfObstacles.push_back(*obj);
	obj = new Object_2D("Ball 2", 2 * off + BALL_RADIUS, off, BALL_RADIUS, 0, 0, glm::vec3(1, 0, 0), true);
	listOfObstacles.push_back(*obj);
	obj = new Object_2D("Ball 3", 3 * off + 2 * BALL_RADIUS, off, BALL_RADIUS, 0, 0, glm::vec3(1, 0, 0), true);
	listOfObstacles.push_back(*obj);

	// PowerUp
	powerUp = new Object_2D("PowerUp", 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, 0, -BALL_SPEED, glm::vec3(0, 1, 0), false);
	listOfObstacles.push_back(*obj);

	// Create the meshes
	Mesh* playerMesh = CreateObject2D::CreateRectangle(player->name, glm::vec3(player->posX, player->posY, 0), player->width, player->height, player->color);
	AddMeshToList(playerMesh);
	Mesh* ballMesh = CreateObject2D::CreateCircle(ball->name, glm::vec3(ball->posX, ball->posY, 0), ball->radius, ball->color);
	AddMeshToList(ballMesh);
	Mesh* powerUpMesh = CreateObject2D::CreateRectangle(powerUp->name, glm::vec3(powerUp->posX, powerUp->posY, 0), powerUp->width, powerUp->height, powerUp->color);
	AddMeshToList(powerUpMesh);
	for (Object_2D obstacle : listOfObstacles) 
	{
		if (obstacle.isBall == false)
		{
			Mesh* obstacleMesh = CreateObject2D::CreateRectangle(obstacle.name, glm::vec3(obstacle.posX, obstacle.posY, 0), obstacle.width, obstacle.height, obstacle.color);
			AddMeshToList(obstacleMesh);
		}
		else
		{
			Mesh* obstacleMesh = CreateObject2D::CreateCircle(obstacle.name, glm::vec3(obstacle.posX, obstacle.posY, 0), obstacle.radius, obstacle.color);
			AddMeshToList(obstacleMesh);
		}
	}
}

void GameEngine::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void GameEngine::Update(float deltaTimeSeconds)
{
	modelMatrix = glm::mat3(1);

	for (int i = 0; i < BLOCKS_PER_COLUMN * BLOCKS_PER_LINE; i++) {
		if (hitBlocks[i] == false) {
			std::string name("Block ");
			name.append(to_string(i));
			RenderMesh2D(meshes[name], shaders["VertexColor"], modelMatrix);
		}
	}
	RenderMesh2D(meshes["Wall Left"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Wall Up"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Wall Right"], shaders["VertexColor"], modelMatrix);

	if (ball->posY < 0) {
		lives--;
		startGame = 0;
		ball->posY = 2 * BALL_GAP + PLAYER_HEIGHT + BALL_RADIUS;
		ball->posX = SCREEN_WIDTH / 2;
		ball->velocityX = 0;
		ball->velocityY = BALL_SPEED;
		player->posY = BALL_GAP;
		player->posX = SCREEN_WIDTH / 2 - player->width / 2;
	}
	for (int i = 1; i <= lives; i++)
		RenderMesh2D(meshes["Ball " + std::to_string(i)], shaders["VertexColor"], modelMatrix);

	playerMatrix = glm::mat3(1);
	playerMatrix *= Transformation2D::Translate(player->posX, 0);
	RenderMesh2D(meshes["Player"], shaders["VertexColor"], playerMatrix);

	ballMatrix = glm::mat3(1);
	if (!startGame) {
		// Initialize variables
		ball->radius = BALL_RADIUS;
		scaleX = 1;
		scaleY = 1;
		totalTime = 0;
		powerupReleased = false;
		ball->posY = 2 * BALL_GAP + PLAYER_HEIGHT + BALL_RADIUS;
	}
	else {
		totalTime++;
		if (totalTime > 30 * 60)
		{
			scaleX = 1;
			scaleY = 1;
			ball->radius = BALL_RADIUS;
			totalTime = 0;
			powerupReleased = false;
		}
		if (GameEngine::CheckCollision(player->posX, player->posY, player->width, player->height)) {
			float point = closestPoint(player->posX, player->posY, player->width, player->height);
			ball->velocityY = sin(point * PI / 180) * BALL_SPEED;
			ball->velocityX = cos(point * PI / 180) * BALL_SPEED;
			ball->posY += ball->velocityY * deltaTimeSeconds;
			ball->posX += ball->velocityX * deltaTimeSeconds;
		}
		else {
			bool hit = false;
			for (Object_2D obstacle : listOfObstacles) {
				if (obstacle.name.find("Ball ") == std::string::npos) {
					if (GameEngine::CheckCollision(obstacle.posX, obstacle.posY, obstacle.width, obstacle.height)) {
						if (obstacle.name.find("Block ") != std::string::npos) {
							if (hitBlocks[atoi(obstacle.name.substr(6, 9).c_str())] == true)
								continue;
							hitBlocks[atoi(obstacle.name.substr(6, 9).c_str())] = true;

							// A block was hit. Maybe a PowerUp can appear.
							if (totalTime > 15 * 60 && powerupReleased == false)
							{
								printf("POWER UP TIME\n");
								powerUp->posX = obstacle.posX;
								powerUp->posY = obstacle.posY;
								powerupReleased = true;
							}
						}

						if (ball->posY >= obstacle.posY && ball->posY <= obstacle.posY + obstacle.height)
							ball->velocityX *= -1;
						else
							ball->velocityY *= -1;
						hit = true;
						break;
					}
				}
			}
			if (hit == false) {
				ball->posY += ball->velocityY * deltaTimeSeconds;
				ball->posX += ball->velocityX * deltaTimeSeconds;
			}
			else {
				ball->posY += 2 * ball->velocityY * deltaTimeSeconds;
				ball->posX += 2 * ball->velocityX * deltaTimeSeconds;
			}
		}

		if (powerupReleased == true)
		{
			if (GameEngine::PowerUpCollision())
			{
				// Apply powerUp
				powerUp->posY = 0;
				ball->radius = POWER_UP_SCALE * BALL_RADIUS;
				scaleX = POWER_UP_SCALE;
				scaleY = POWER_UP_SCALE;
			}
			else if (powerUp->posY > 0)
			{
				powerUpMatrix = glm::mat3(1);
				powerUp->posY += powerUp->velocityY * deltaTimeSeconds;
				powerUp->posX += powerUp->velocityX * deltaTimeSeconds;
				powerUpMatrix *= Transformation2D::Translate(powerUp->posX, powerUp->posY);
				RenderMesh2D(meshes["PowerUp"], shaders["VertexColor"], powerUpMatrix);
			}
		}
	}
	ballMatrix *= Transformation2D::Translate(ball->posX, ball->posY);
	ballMatrix *= Transformation2D::Scale(scaleX, scaleY);
	RenderMesh2D(meshes["Ball 0"], shaders["VertexColor"], ballMatrix);

}


bool GameEngine::CheckCollision(float posX, float posY, float width, float height)
{
	glm::vec2 ballCenter(ball->posX, ball->posY);
	glm::vec2 objCenter(width / 2, height / 2);
	glm::vec2 objCenterPosition(posX + width / 2, posY + height / 2);

	glm::vec2 difference = ballCenter - objCenterPosition;
	glm::vec2 clamped = glm::clamp(difference, -objCenter, objCenter);
	glm::vec2 closest = objCenterPosition + clamped;
	difference = closest - ballCenter;
	return length(difference) < ball->radius;

}

float GameEngine::closestPoint(float posX, float posY, float width, float height)
{
	glm::vec2 ballCenter(ball->posX, ball->posY);
	glm::vec2 objCenter(width / 2, height / 2);
	glm::vec2 objCenterPosition(posX + width / 2, posY + height / 2);

	glm::vec2 difference = ballCenter - objCenterPosition;
	glm::vec2 clamped = glm::clamp(difference, -objCenter, objCenter);
	return -clamped.x + 90;
}

bool GameEngine::PowerUpCollision()
{
	bool collisionX = powerUp->posX + powerUp->width >= player->posX &&
					  player->posX  + player->width  >= powerUp->posX;
	bool collisionY = powerUp->posY + powerUp->height >= player->posY &&
					  player->posY  + player->height  >= powerUp->posY;
	return collisionX && collisionY;
}

void GameEngine::FrameEnd()
{

}

void GameEngine::OnInputUpdate(float deltaTime, int mods)
{

}

void GameEngine::OnKeyPress(int key, int mods)
{
	// add key press event
}

void GameEngine::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void GameEngine::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	player->posX = mouseX - player->width/2;
	if (startGame == 0)
		ball->posX = (float)mouseX;
}

void GameEngine::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	startGame = 1;
}

void GameEngine::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void GameEngine::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void GameEngine::OnWindowResize(int width, int height)
{
}
