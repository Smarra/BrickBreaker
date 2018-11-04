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

	// Background
	Mesh* bgMesh = CreateObject2D::CreateRectangle("BG", glm::vec3(0, 0, -0.1), SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0.1, 0, 0.1));
	AddMeshToList(bgMesh);

	// Initialize some variables (arrays that check if the blocks or shields are hit)
	Object_2D *obj;
	startGame = 0;
	for (int i = 0; i < BLOCKS_PER_COLUMN * BLOCKS_PER_LINE; i++)
	{
		hitBlocks.push_back(false);
		hitShield.push_back(false);
		scaleBlock.push_back(1);
	}

	// Creates the objects and meshes for the following types of objects:
	// Blocks
	float ys = SCREEN_HEIGHT - 100.0f;
	float xs = (SCREEN_WIDTH - BLOCKS_PER_LINE * (BLOCK_WIDTH + BLOCK_GAP) + BLOCK_GAP) / 2;
	for (int i = 0; i < BLOCKS_PER_COLUMN; i++)
	{
		vector<Object_2D> line;
		for (int j = 0; j < BLOCKS_PER_LINE; j++) 
		{
			int index = i * BLOCKS_PER_LINE + j;

			// Create the block
			std::string name("Block " + to_string(index));
			obj = new Object_2D(name, xs, ys, BLOCK_WIDTH, BLOCK_HEIGHT, 0, 0, glm::vec3(i /BLOCKS_PER_LINE / 1.5 + 0.3, j / BLOCKS_PER_COLUMN / 8, 0), false);
			listOfObstacles.push_back(*obj);
			blocks.push_back(*obj);

			// Create the shield
			std::string name2("Shield " + to_string(index));
			obj = new Object_2D(name2, xs - SHIELD_OFFSET/2, ys - SHIELD_OFFSET/2, BLOCK_WIDTH + SHIELD_OFFSET, BLOCK_HEIGHT + SHIELD_OFFSET, 0, 0, glm::vec3(1, 0.5, 0), false);
			listOfObstacles.push_back(*obj);
			line.push_back(*obj);

			// Increment x position
			xs += BLOCK_GAP + BLOCK_WIDTH;
		}
		shields.push_back(line);

		// Actualize the y position and reset the x position
		ys -= BLOCK_GAP + BLOCK_HEIGHT;
		xs = (SCREEN_WIDTH - BLOCKS_PER_LINE * (BLOCK_WIDTH + BLOCK_GAP) + BLOCK_GAP) / 2;
	}
	
	// Walls
	float off = 2 * BALL_GAP + PLAYER_HEIGHT;
	obj = new Object_2D("Wall Left", 0, off, WALL_WIDTH, SCREEN_HEIGHT - off, 0, 0, glm::vec3(0.3, 0.3, 0.4), false);
	listOfObstacles.push_back(*obj);
	obj = new Object_2D("Wall Up", 0, SCREEN_HEIGHT - WALL_WIDTH, SCREEN_WIDTH, WALL_WIDTH, 0, 0, glm::vec3(0.3, 0.3, 0.4), false);
	listOfObstacles.push_back(*obj);
	obj = new Object_2D("Wall Right", SCREEN_WIDTH - WALL_WIDTH, off, WALL_WIDTH, SCREEN_HEIGHT - off, 0, 0, glm::vec3(0.3, 0.3, 0.4), false);
	listOfObstacles.push_back(*obj);
	pWall = new Object_2D("Wall Down", 0, 0, SCREEN_WIDTH, WALL_WIDTH, 0, 0, glm::vec3(0.3, 0.3, 0.4), false);
	Mesh* eMesh = CreateObject2D::CreateRectangle(pWall->name, glm::vec3(pWall->posX, pWall->posY, 0), pWall->width, pWall->height, pWall->color);
	AddMeshToList(eMesh);

	// Player
	player = new Object_2D("Player", 0, BALL_GAP, PLAYER_WIDTH, PLAYER_HEIGHT, 0, 0, glm::vec3(0.5, 0.1, 0.1), false);

	// Ball
	off = BALL_RADIUS + BALL_GAP;
	ball = new Object_2D("Ball 0", 0, 0, BALL_RADIUS, 0, BALL_SPEED, glm::vec3(1, 0, 0), true);

	// Lives
	Mesh* ballMesh;
	obj = new Object_2D("Ball 1", off, off, BALL_RADIUS, 0, 0, glm::vec3(1, 0, 0), true);
	ballMesh = CreateObject2D::CreateCircle(obj->name, glm::vec3(obj->posX, obj->posY, 0), obj->radius, obj->color);
	AddMeshToList(ballMesh);
	obj = new Object_2D("Ball 2", 2 * off + BALL_RADIUS, off, BALL_RADIUS, 0, 0, glm::vec3(1, 0, 0), true);
	ballMesh = CreateObject2D::CreateCircle(obj->name, glm::vec3(obj->posX, obj->posY, 0), obj->radius, obj->color);
	AddMeshToList(ballMesh);
	obj = new Object_2D("Ball 3", 3 * off + 2 * BALL_RADIUS, off, BALL_RADIUS, 0, 0, glm::vec3(1, 0, 0), true);
	ballMesh = CreateObject2D::CreateCircle(obj->name, glm::vec3(obj->posX, obj->posY, 0), obj->radius, obj->color);
	AddMeshToList(ballMesh);

	// PowerUp
	powerUp = new Object_2D("PowerUp", 0, 0, POWER_UP_WIDTH, POWER_UP_WIDTH, 0, -BALL_SPEED/2, glm::vec3(0.5, 0.8, 0), false);
	listOfObstacles.push_back(*obj);

	// Create the meshes
	Mesh* playerMesh = CreateObject2D::CreateRectangle(player->name, glm::vec3(player->posX, player->posY, 0), player->width, player->height, player->color);
	AddMeshToList(playerMesh);
	ballMesh = CreateObject2D::CreateCircle(ball->name, glm::vec3(ball->posX, ball->posY, 0), ball->radius, ball->color);
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
	int poz = 0;
	modelMatrix = glm::mat3(1);
	ballMatrix = glm::mat3(1);
	playerMatrix = glm::mat3(1);

	// Render Background
	RenderMesh2D(meshes["BG"], shaders["VertexColor"], modelMatrix);

	// If the ball has fallen under the player, we decrement the number of lives.
	// If the lives reach 0, the game is over
	if (ball->posY < 0) {
		lives--;
		if (lives <= 0)
			return;
		resetGame();
	}

	// Checks if the game is finished and the player has completed the level. 
	// In the succesful case, we move to the next level.
	if (blocksHit == BLOCKS_PER_COLUMN * BLOCKS_PER_LINE)
	{
		level = (level + 1) % ((int)BLOCKS_PER_LINE + 1);
		resetGame();
	}

	// Render the meshes for the shields based on the level we are on and if the shield was hit or not
	for (int i = shields.size() - level; i <= shields.size() - 1; i++)
		for (int j = 0; j < shields[i].size(); j++)
			if (hitShield[i * BLOCKS_PER_LINE + j] == false)
				RenderMesh2D(meshes[shields[i][j].name], shaders["VertexColor"], modelMatrix);

	// Render the meshes for the blocks. If the block was hit, starts a process of scaling down the block until it disappears completely
	for (Object_2D block : blocks)
	{
		blockMatrix = glm::mat3(1);
		if (hitBlocks[poz] == false)
			RenderMesh2D(meshes[block.name], shaders["VertexColor"], blockMatrix);
		else
			if (scaleBlock[poz] > 0)
			{
				blockMatrix *= Transformation2D::Translate(block.posX + block.width / 2, block.posY + block.height / 2);
				blockMatrix *= Transformation2D::Scale(scaleBlock[poz], scaleBlock[poz]);
				blockMatrix *= Transformation2D::Translate(-block.posX - block.width / 2, -block.posY - block.height / 2);
				RenderMesh2D(meshes[block.name], shaders["VertexColor"], blockMatrix);
				scaleBlock[poz] -= 0.1;
			}
			else
				scaleBlock[poz] = 0;
		poz++;
	}

	// Render the walls
	RenderMesh2D(meshes["Wall Left"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Wall Up"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["Wall Right"], shaders["VertexColor"], modelMatrix);
	if (extraWall == true)
		RenderMesh2D(meshes["Wall Down"], shaders["VertexColor"], modelMatrix);

	// Render the lives in the left corner of the screen
	for (int i = 1; i <= lives; i++)
		RenderMesh2D(meshes["Ball " + std::to_string(i)], shaders["VertexColor"], modelMatrix);

	// Render the player, constantly translating its X axis on the mouse axis
	playerMatrix *= Transformation2D::Translate(player->posX, 0);
	RenderMesh2D(meshes["Player"], shaders["VertexColor"], playerMatrix);

	// The gameplay starts. First we check if we are in a state of waiting the input from the user.
	// In the variable startGame we store if the game is on or not.
	if (!startGame) {
		// Initialize variables
		initPowerUp();
		ball->posY = 2 * BALL_GAP + PLAYER_HEIGHT + BALL_RADIUS;
		blocksHit = 0;
	}
	else {
		// totalTime checks 2 things:
		// 1. How much time has passed since the last powerUp (min 5 seconds)
		// 2. Or how much time passed since the activation of the powerUp (min 15 seconds)
		// A powerUp is activated if the min time has passed and if a block was hit.
		totalTime++;
		if (totalTime > 15 * 60)
			initPowerUp();

		// Now we are going to check for collisions with the ball.
		// Firstly, with the wall that appears when the powerUp specified my id is activated
		if (extraWall && GameEngine::CheckCollision(pWall->posX, pWall->posY, pWall->width, pWall->height))
		{
			ball->velocityY *= -1;
			ball->posY += ball->velocityY * deltaTimeSeconds;
			ball->posX += ball->velocityX * deltaTimeSeconds;
		}

		// Secodnly, with the player
		else if (GameEngine::CheckCollision(player->posX, player->posY, player->width, player->height)) {
			float point = closestPoint(player->posX, player->posY, player->width, player->height);
			ball->velocityY = sin(point * PI / 180) * BALL_SPEED;
			ball->velocityX = cos(point * PI / 180) * BALL_SPEED;
			ball->posY += ball->velocityY * deltaTimeSeconds;
			ball->posX += ball->velocityX * deltaTimeSeconds;
		}

		// Lastly, with the remaining obstacles: blocks, walls and shields.
		// After that, we use the hit variable to verify if the ball has hit something.
		else {
			bool hit = false;
			for (Object_2D obstacle : listOfObstacles)
				if (GameEngine::CheckCollision(obstacle.posX, obstacle.posY, obstacle.width, obstacle.height)) {

					// Check if the hit onject is a block
					if (obstacle.name.find("Block ") != std::string::npos) {
						int nrBlock = atoi(obstacle.name.substr(6, 9).c_str());
						if (hitBlocks[nrBlock] == true)
							continue;

						hitBlocks[nrBlock] = true;
						blocksHit++;
						// A block was hit. Maybe a PowerUp can appear.
						if (totalTime > 5 * 60 && powerupReleased == false)
						{
							totalTime = 0;
							powerUp->posX = obstacle.posX;
							powerUp->posY = obstacle.posY;
							powerupReleased = true;
							powerUpId = 1 + rand() % 2;
						}
					}

					// Check if the hit onject is a block
					else if (obstacle.name.find("Shield ") != std::string::npos) {
						int nrBlock = atoi(obstacle.name.substr(7, 10).c_str());
						int i = nrBlock / BLOCKS_PER_LINE;
						int j = nrBlock % (int)BLOCKS_PER_LINE;
						if (i <= shields.size() - 1 && i >= shields.size() - level && hitShield[nrBlock] == false)
							hitShield[nrBlock] = true;
						else
							continue;
					}
					
					// Actualize the velocity as the ball hit an obstacle different than the player
					if (ball->posY >= obstacle.posY && ball->posY <= obstacle.posY + obstacle.height)
						ball->velocityX *= -1;
					else
						ball->velocityY *= -1;
					hit = true;
					break;
				}

			// Actualize the position. If the ball hit an obstacle, it will increase by 2 * velocity to avoid
			// being stuck in an object because of the frame rate. Otherwise, it will increase normally.
			// OBS: The ball increases by 2 * velocoty only for a frame (when it hits an object).
			if (hit == false) {
				ball->posY += ball->velocityY * deltaTimeSeconds;
				ball->posX += ball->velocityX * deltaTimeSeconds;
			}
			else {
				ball->posY += 2 * ball->velocityY * deltaTimeSeconds;
				ball->posX += 2 * ball->velocityX * deltaTimeSeconds;
			}


			// This part deals with the case when the powerUp is released, but not activated yet.
			if (powerupReleased == true)
			{
				// This checks if the player got the powerUp or not.
				// If so, the powerUp will be applied based on its id.
				// Otherwise, we actualize the position of the powerUp.
				if (GameEngine::PowerUpCollision())
				{
					powerUp->posY = -100;
					powerUp->posX = -100;

					// Apply powerUp
					if (powerUpId == 1)
					{
						ball->radius = POWER_UP_SCALE * BALL_RADIUS;
						scaleX = POWER_UP_SCALE;
						scaleY = POWER_UP_SCALE;
					}
					else if (powerUpId == 2)
						extraWall = true;
				}
				else if (powerUp->posY > 0)
				{
					powerUpMatrix = glm::mat3(1);
					powerUp->posY += powerUp->velocityY * deltaTimeSeconds;
					powerUp->posX += powerUp->velocityX * deltaTimeSeconds;
					powerUpMatrix *= Transformation2D::Translate(powerUp->posX, powerUp->posY);
					powerUpMatrix *= Transformation2D::Rotate(totalTime * PI / 180);
					powerUpMatrix *= Transformation2D::Translate(0, 0);
					RenderMesh2D(meshes["PowerUp"], shaders["VertexColor"], powerUpMatrix);
				}
				else
				{
					powerUp->posY = -100;
					powerUp->posX = -100;
				}
			}
		}
	}

	// After obtaining the next position of the ball based on eventually collisions,
	// we apply that position and actualize the position of the ball and scale it accordingly
	ballMatrix *= Transformation2D::Translate(ball->posX, ball->posY);
	ballMatrix *= Transformation2D::Scale(scaleX, scaleY);
	RenderMesh2D(meshes["Ball 0"], shaders["VertexColor"], ballMatrix);
}

void GameEngine::initPowerUp()
{
	// Basic variables for the powerUp
	totalTime = 0;
	powerupReleased = false;
	powerUpId = 0;

	//PowerUp 1
	scaleX = 1;
	scaleY = 1;
	ball->radius = BALL_RADIUS;
	
	// PowerUp 2
	extraWall = false;
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

void GameEngine::resetGame()
{
	startGame = 0;
	ball->posY = 2 * BALL_GAP + PLAYER_HEIGHT + BALL_RADIUS;
	ball->posX = SCREEN_WIDTH / 2;
	ball->velocityX = 0;
	ball->velocityY = BALL_SPEED;
	player->posY = BALL_GAP;
	player->posX = SCREEN_WIDTH / 2 - player->width / 2;

	initPowerUp();
	blocksHit = 0;
}

void GameEngine::FrameEnd()
{

}

void GameEngine::OnInputUpdate(float deltaTime, int mods)
{

}

void GameEngine::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_N)
	{
		lives = 3;
		level = 0;
		for (int i = 0; i < BLOCKS_PER_COLUMN * BLOCKS_PER_LINE; i++)
		{
			hitBlocks[i] = false;
			hitShield[i] = false;
		}
		resetGame();

	}

	if (key == GLFW_KEY_I)
	{
		lives = 3;
		level = (level + 1) % ((int)BLOCKS_PER_COLUMN + 1);
		for (int i = 0; i < BLOCKS_PER_COLUMN * BLOCKS_PER_LINE; i++)
		{
			hitBlocks[i] = false;
			hitShield[i] = false;
		}
		resetGame();
	}
}

void GameEngine::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void GameEngine::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (mouseX < 50)
		mouseX = 50;
	if (mouseX > SCREEN_WIDTH - 50)
		mouseX = SCREEN_WIDTH - 50;
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
