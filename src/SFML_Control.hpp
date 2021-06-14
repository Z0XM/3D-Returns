#pragma once
#include <SFML/Graphics.hpp>
#include "Renderer3D.hpp"

class SFML_Control {
private:
	sf::RenderWindow window;
	Renderer3D* renderer;
	bool running;
	bool isPaused;

	sf::Clock clock;

	void InitWindow();
public:
	SFML_Control();
	~SFML_Control();

	bool isRunning();

	void update();
	void pollEvents();
	void render();

	void drawAxis();
	sf::Vector2f getMousePosition();
	sf::Vector2f getWinSize();
};
