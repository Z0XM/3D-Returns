#pragma once
#include <SFML/Graphics.hpp>
#include "Utility.hpp"

class Renderer3D{
	enum class RENDER_MODE{
		PIXEL, TRIANGLE
	} renderingMode;

	Mesh cube;
	Matrix4x4 matProj;
	Vector camera, lookDir;

	float fTheta, yaw;

	int pixel_y, pixel_x;
	sf::Vertex* pixels;
	float* zBuffer;

	std::vector<Triangle> trianglestoRaster;

	void clearPixels();
	void updatePixel(const Vector& v, const sf::Color& c);
	void drawTriangleToPixels(const Triangle& tri);
	void drawPixels(sf::RenderWindow& window);

	void drawTriangles(sf::RenderWindow& window);

public:
	Renderer3D(const sf::Vector2u& winSize);
	~Renderer3D();

	void scaleToView(Triangle& tri,const sf::Vector2f& winSize);

	void clear();
	void update(float dt);
	void render(sf::RenderWindow& window);
};