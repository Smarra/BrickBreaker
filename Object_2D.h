#ifndef Object_2D_HEADER
#define Object_2D_HEADER

#include <string>
#include <include/glm.h>

using namespace std;

class Object_2D {
public:
	float posX, posY;
	float width, height;
	float velocityX, velocityY;
	float radius;
	bool isBall;
	std::string name;
	glm::vec3 color;
	Object_2D();
	~Object_2D();
	Object_2D(std::string name, float posX, float posY, float width, float height, float velocityX, float velocityY, glm::vec3 color, bool isBall);
	Object_2D(std::string name, float posX, float posY, float radius, float velocityX, float velocityY, glm::vec3 color, bool isBall);
	float getBallPositionAfterImpact(Object_2D *ball);
};

#endif