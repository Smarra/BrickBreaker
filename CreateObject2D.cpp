#include "CreateObject2D.h"

Mesh* CreateObject2D::CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float length, float height, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* rectangle = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3, 0, 2 };

	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

Mesh* CreateObject2D::CreateCircle(std::string name, glm::vec3 center, float radius, glm::vec3 color)
{
	glm::vec3 corner = center;

	std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(corner, color));
	for (int i = 0; i < 360; i++)
		vertices.push_back(VertexFormat(corner + glm::vec3(radius * cos(i), radius * sin(i), 0), color));

	Mesh* circle = new Mesh(name);
	std::vector<unsigned short> indices;

	for (int i = 1; i < 360; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}
