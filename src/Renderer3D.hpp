#pragma once
#include <SFML/Graphics.hpp>
#include "Utility.hpp"

class Renderer3D{
	enum class RENDER_MODE{
		PIXEL, TRIANGLE
	} renderingMode;

	Mesh cube;
	Matrix4x4 matProj;
	Vector camera;

	int pixel_y, pixel_x;
	sf::Vertex* pixels;
	float* zBuffer;

	float fTheta;

	void drawTriangleToPixels(const Triangle& tri);
	void clearPixels();
	void updatePixels(float dt);
	void drawPixels(sf::RenderWindow& window);
	void drawTriangles(sf::RenderWindow& window, float dt);

	void fillBottomFlatTriangle(const Triangle& tri);
	void fillTopFlatTriangle(const Triangle& tri);
	void updatePixel(const Vector& v, const sf::Color& c);

public:
	Renderer3D(const sf::Vector2u& winSize);
	~Renderer3D();

	void scaleToView(Triangle& tri,const sf::Vector2f& winSize);

	void render(sf::RenderWindow& window, float dt);
};