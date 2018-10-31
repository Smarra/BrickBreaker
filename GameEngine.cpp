#include "GameEngine.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include "Transformation2D.h"
#include "Object_2D.h"
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

	// Offset for the translation of the ball in the beginning
	velocityY = BALL_SPEED;
	velocityX = -80 + rand() % 160;

	// start game
	startGame = 0;

	// Blocks
	float ys = SCREEN_HEIGHT - 100.0f;
	float xs = (SCREEN_WIDTH - BLOCKS_PER_LINE * (BLOCK_WIDTH + BLOCK_GAP) + BLOCK_GAP) / 2;
	for (int i = 0; i < BLOCKS_PER_COLUMN; i++) {
		for (int j = 0; j < BLOCKS_PER_LINE; j++) {
			int index = i * BLOCKS_PER_LINE + j;
			std::string name("Block " + to_string(index));
			Mesh* block = CreateObject2D::CreateRectangle(name, glm::vec3(xs, ys, 0), BLOCK_WIDTH, BLOCK_HEIGHT, glm::vec3(1, 0, 0), true);
			AddMeshToList(block);
			xs += BLOCK_GAP + BLOCK_WIDTH;
		}
		ys -= BLOCK_GAP + BLOCK_HEIGHT;
		xs = (SCREEN_WIDTH - BLOCKS_PER_LINE * (BLOCK_WIDTH + BLOCK_GAP) + BLOCK_GAP) / 2;
	}

	// Walls
	Mesh* wall;
	float off = 2 * OFFSET_PLAYER_WALL + PLAYER_HEIGHT;
	wall = CreateObject2D::CreateRectangle("Wall Left", glm::vec3(0, off, 0), WALL_WIDTH, SCREEN_HEIGHT - off, glm::vec3(1, 0, 0), true);
	AddMeshToList(wall);
	wall = CreateObject2D::CreateRectangle("Wall Up", glm::vec3(0, SCREEN_HEIGHT - WALL_WIDTH, 0), SCREEN_WIDTH, WALL_WIDTH, glm::vec3(1, 0, 0), true);
	listOfObstacles.push_back(std::make_tuple("Wall Up", 0, SCREEN_HEIGHT - WALL_WIDTH));
	AddMeshToList(wall);
	wall = CreateObject2D::CreateRectangle("Wall Right", glm::vec3(SCREEN_WIDTH - WALL_WIDTH, off, 0), WALL_WIDTH, SCREEN_HEIGHT - off, glm::vec3(1, 0, 0), true);
	AddMeshToList(wall);

	// Player
	Mesh* player = CreateObject2D::CreateRectangle("Player", glm::vec3(0, OFFSET_PLAYER_WALL, 0), PLAYER_WIDTH, PLAYER_HEIGHT, glm::vec3(1, 0, 0), true);
	AddMeshToList(player);

	// Balls
	Mesh* ball;
	off = BALL_RADIUS + BALL_GAP;
	ball = CreateObject2D::CreateCircle("Ball 0", glm::vec3(0, 0, 0),
									BALL_RADIUS, glm::vec3(0, 1, 0));
	AddMeshToList(ball);
	ball = CreateObject2D::CreateCircle("Ball 1", glm::vec3(off, off, 0), BALL_RADIUS, glm::vec3(0, 1, 0));
	AddMeshToList(ball);
	ball = CreateObject2D::CreateCircle("Ball 2", glm::vec3(2 * off + BALL_RADIUS, off, 0), BALL_RADIUS, glm::vec3(0, 1, 0));
	AddMeshToList(ball);
	ball = CreateObject2D::CreateCircle("Ball 3", glm::vec3(3 * off + 2 * BALL_RADIUS, off, 0), BALL_RADIUS, glm::vec3(0, 1, 0));
	AddMeshToList(ball);
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

	for (int i = 0; i < BLOCKS_PER_COLUMN * BLOCKS_PER_LINE; i++)
	{
		std::string name = "Block " + to_string(i);
		RenderMesh2D(meshes[name], shaders["VertexColor"], modelMatrix);
	}
	RenderMesh2D(meshes["Wall Left"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Wall Up"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Wall Right"], shaders["VertexColor"], modelMatrix);

	RenderMesh2D(meshes["Ball 1"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Ball 2"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Ball 3"], shaders["VertexColor"], modelMatrix);

	playerMatrix = glm::mat3(1);
	playerMatrix *= Transformation2D::Translate(mousePosX - PLAYER_WIDTH/2, 0);
	RenderMesh2D(meshes["Player"], shaders["VertexColor"], playerMatrix);

	ballMatrix = glm::mat3(1);
	if (!startGame)
	{
		ballPosX = mousePosX;
		ballPosY = 0;
		ballMatrix *= Transformation2D::Translate(mousePosX, 0);
		RenderMesh2D(meshes["Ball 0"], shaders["VertexColor"], ballMatrix);
	}
	else
	{
		for (std::tuple<std::string, float, float> obstacle : listOfObstacles)
		{
			string name = std::get<0>(obstacle);
			float auxX = std::get<1>(obstacle);
			float auxY = std::get<2>(obstacle);
			if (name.compare("Wall Up") == 0)
			{
				//printf("%f %f %f %f\n", auxX, auxY, SCREEN_WIDTH, WALL_WIDTH);
				if (GameEngine::CheckCollision(auxX, auxY, SCREEN_WIDTH, WALL_WIDTH))
					velocityY *= -1;
				ballPosY += velocityY * deltaTimeSeconds;
				ballPosX += velocityX * deltaTimeSeconds;
				ballMatrix *= Transformation2D::Translate(ballPosX, ballPosY);
				RenderMesh2D(meshes["Ball 0"], shaders["VertexColor"], ballMatrix);
			}
		}
	}

}


GLboolean GameEngine::CheckCollision(float posX, float posY, float width, float height)
{
	glm::vec2 ballCenter(ballPosX, ballPosY);
	glm::vec2 objCenter(width / 2, height / 2);
	glm::vec2 objCenterPosition(posX + width / 2, posY + height / 2);

	glm::vec2 difference = ballCenter - objCenterPosition;
	glm::vec2 clamped = glm::clamp(difference, -objCenter, objCenter);
	glm::vec2 closest = objCenterPosition + clamped;
	difference = closest - ballCenter;
	return glm::length(difference) < BALL_RADIUS;

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
	mousePosX = mouseX;
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
