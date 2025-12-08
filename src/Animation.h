#pragma once
#include "SFML/Graphics.hpp"

class Animation
{
private:
	std::string m_name = "NONE";
	size_t m_keyframesCount = 1;
	size_t m_duration = 1;
	size_t m_currentKeyframe = 0;
	size_t m_gameFrames = 0;

	sf::Vector2f m_size = { 1,1 };
	sf::Sprite m_sprite;

public:
	Animation();
	Animation(const std::string& name, const sf::Texture& texture);
	Animation(const std::string& name, const sf::Texture& texture,
		size_t keyframesCount, size_t duration);

	sf::Sprite& getSprite();
	void changeColor(sf::Color);
	const std::string& getName()   const;
	const sf::Vector2f& getSize()   const;

	void update();
	bool hasEnded() const;
};

