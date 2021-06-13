#pragma once

#include <SFML/Graphics.hpp>

class BlinkingColors {
	sf::Color current;
	
	static int colorArrSize;
	static sf::Color colorArr[6];

public:
	BlinkingColors();

	void next();
	void prev();
	void reset();

	sf::Color getColor();

	static sf::Color next(sf::Color);
	static sf::Color random();
};
