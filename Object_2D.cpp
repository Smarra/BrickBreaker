#include "Object_2D.h"

Object_2D::Object_2D() {

}

Object_2D::~Object_2D() {

}

Object_2D::Object_2D(float posX, float posY, float width, float height, float velocityX, float velocityY)
{
	this->posX = posX;
	this->posY = posY;
	this->width = width;
	this->height = height;
	this->velocityX = velocityX;
	this->velocityY = velocityY;
}
