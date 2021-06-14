#pragma once
#include <SFML/Graphics.hpp>
#include "Utility.hpp"

class Renderer3D{
	sf::Vector2f winSize;

	Mesh cube;
	Matrix4x4 matProj;
	Vector camera, lookDir;

	float fTheta, yaw;

	std::vector<Triangle> trianglestoRaster;

	void drawTriangles(sf::RenderWindow& window);

public:
	Renderer3D(const sf::Vector2u& winSize);
	~Renderer3D();

	void clear();
	void update(float dt);
	void render(sf::RenderWindow& window);
};