#pragma once
#include "SFML/Graphics.hpp"
#include "Entity.h"

class Physics
{
private:
public:
	sf::Vector2f GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	sf::Vector2f GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
};

