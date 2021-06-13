#include "BlinkingColors.hpp"

int BlinkingColors::colorArrSize = 6;
sf::Color BlinkingColors::colorArr[6] =
{
	sf::Color(255,255,255,255),// White
	sf::Color(255,255,0,255),  // Yellow
	sf::Color(255,0,0,255),    // Red
	sf::Color(255,0,255,255),  // Magenta
	sf::Color(0,0,255,255),    // Blue
	sf::Color(0,255,255,255)   // Cyan
};

BlinkingColors::BlinkingColors()
{
	current = sf::Color::White;
}

void BlinkingColors::next()
{
	for (int i = 0; i < colorArrSize; i++) {
		if (current == colorArr[i]) {
			current = colorArr[(i + 1) % colorArrSize];
			break;
		}
	}
}

void BlinkingColors::prev()
{
	for (int i = 0; i < colorArrSize; i++) {
		if (current == colorArr[i]) {
			current = colorArr[(i - 1 + colorArrSize) % colorArrSize];
			break;
		}
	}
}

void BlinkingColors::reset()
{
	current = sf::Color::White;
}

sf::Color BlinkingColors::getColor()
{
	return current;
}

sf::Color BlinkingColors::next(sf::Color color)
{
	for (int i = 0; i < colorArrSize; i++) {
		if (color == colorArr[i])return colorArr[(i + 1) % colorArrSize];
	}
	return sf::Color::White;
}

sf::Color BlinkingColors::random()
{
	return colorArr[rand() % colorArrSize];
}