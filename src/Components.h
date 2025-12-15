#pragma once
#include "Animation.h"

class Component
{
public:
	bool has = false;
};

class CScore : public Component
{
public:
	int coins = 0;

	CScore() {};
};

class CTransform : public Component
{
public:
	sf::Vector2f prevPos = { 0.0f,0.0f };
	sf::Vector2f pos = { 0.0f,0.0f };
	sf::Vector2f velocity = { 0.0f,0.0f };
	sf::Vector2f scale = { 0.0f,0.0f };
	float angle = 0;

	CTransform() {}

	CTransform(const sf::Vector2f& p) : pos(p) {}

	CTransform(const sf::Vector2f& _pos, const sf::Vector2f& _velocity, const sf::Vector2f& _scale,
		float _angle)
		: pos(_pos)
		, prevPos(_pos)
		, velocity(_velocity)
		, scale(_scale)
		, angle(_angle)
	{
	}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput()
	{
	}
};

class CLifespan : public Component
{
public:
	int remaining = 0;
	int total = 0;

	CLifespan() {}
	CLifespan(const int& _lifespan) : total(_lifespan)
	{
		remaining = total;
	}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;

	CAnimation()
	{
	}

	CAnimation(const Animation& _animation, bool _repeat)
		: animation(_animation)
		, repeat(_repeat)
	{
	}
};

class CGravity : public Component
{
public:
	float gravity = 0;

	CGravity()
	{
	}

	CGravity(float g)
		: gravity(g)
	{
	}
};

class CGameplayTags : public Component
{
public:
	
	std::vector<std::string> gameplayTags;
	CGameplayTags()
	{
	}

	CGameplayTags(const std::string& _tag)
		
	{
		gameplayTags.push_back(_tag);
	}
};

class CBoundingBox : public Component
{
public:
	sf::Vector2f size;
	sf::Vector2f halfSize;
	CBoundingBox()
	{
	}

	CBoundingBox(const sf::Vector2f& _size)
		: size(_size), halfSize(_size.x / 2.0f, _size.y / 2.0f)
	{
	}
};