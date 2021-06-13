#include "SFML_Control.hpp"
#include <iostream>

void SFML_Control::InitWindow()
{
	window.create(sf::VideoMode(1200, 700), "3D Returns");
	window.setFramerateLimit(60);
}

SFML_Control::SFML_Control()
{
	this->InitWindow();

	this->renderer = new Renderer3D(this->window.getSize());

	this->running = true;
}

SFML_Control::~SFML_Control()
{
}

bool SFML_Control::isRunning()
{
	return this->running;
}

void SFML_Control::update()
{
	this->pollEvents();
}

void SFML_Control::pollEvents()
{
	sf::Event event;
	sf::Vector2f mousePos = this->getMousePosition();
	while (this->window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			this->running = false;

		if (event.type == sf::Event::MouseButtonPressed) {

		}
		else if (event.type == sf::Event::MouseButtonReleased) {

		}
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape)
				this->running = false;
		}
	}
}

void SFML_Control::render()
{
	this->window.clear();

	this->renderer->render(this->window, this->clock.getElapsedTime().asSeconds());
	this->clock.restart();

	//this->drawAxis();

	this->window.display();
}

void SFML_Control::drawAxis()
{
	sf::Vertex axis[4] = { sf::Vector2f(0, 350), sf::Vector2f(1200, 350), sf::Vector2f(600, 0), sf::Vector2f(600, 700) };
	window.draw(axis, 4, sf::Lines);
}

sf::Vector2f SFML_Control::getMousePosition()
{
	return window.mapPixelToCoords(sf::Mouse::getPosition(this->window));
}

sf::Vector2f SFML_Control::getWinSize()
{
	return sf::Vector2f(this->window.getSize());
}