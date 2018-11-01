#include "Object_2D.h"

Object_2D::Object_2D() {

}

Object_2D::~Object_2D() {

}

Object_2D::Object_2D(std::string name,float posX, float posY, float width, float height, float velocityX, float velocityY, glm::vec3 color, bool isBall)
{
	this->color = color;
	this->name = name;
	this->posX = posX;
	this->posY = posY;
	this->width = width;
	this->height = height;
	this->velocityX = velocityX;
	this->velocityY = velocityY;
	this->isBall = isBall;
}

Object_2D::Object_2D(std::string name, float posX, float posY, float radius, float velocityX, float velocityY, glm::vec3 color, bool isBall)
{
	this->color = color;
	this->name = name;
	this->posX = posX;
	this->posY = posY;
	this->radius = radius;
	this->velocityX = velocityX;
	this->velocityY = velocityY;
	this->isBall = isBall;
}

float Object_2D::getBallPositionAfterImpact(Object_2D* ball)
{
	if (ball->posY < this->posY)
		return this->posY - ball->radius;

}