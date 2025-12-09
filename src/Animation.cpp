#include "Animation.h"

Animation::Animation()
{
}

Animation::Animation(const std::string& name, const sf::Texture& texture)
	: Animation(name, texture, 1, 0)
{
}

Animation::Animation(const std::string& name, const sf::Texture& texture,
	size_t keyframesCount, size_t duration)
	: m_name(name)
	, m_sprite(texture)
	, m_keyframesCount(keyframesCount)
	, m_currentKeyframe(0)
	, m_duration(duration)
{
	m_size = sf::Vector2f(
		(float)texture.getSize().x / keyframesCount,
		(float)texture.getSize().y
	);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f); // just gonna set teh origin to the mid rq
	m_sprite.setTextureRect(
		sf::IntRect(m_currentKeyframe * m_size.x, 0, m_size.x, m_size.y)
	); // gotta make sure the textures are rect for now
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}

void Animation::changeColor(sf::Color c)
{
	m_sprite.setColor(c);
}

void Animation::update()
{
	
	m_gameFrames++;
	if (m_duration == 0)
		return;
	m_currentKeyframe = (m_gameFrames / m_duration) % m_keyframesCount;
	const auto rectange = sf::IntRect(
		m_currentKeyframe * m_size.x, 0, m_size.x, m_size.y
	);
	m_sprite.setTextureRect(rectange);
}

const sf::Vector2f& Animation::getSize() const
{
	return m_size;
}

const size_t Animation::getKeyframeCount() const
{
	return m_keyframesCount;
}

const std::string& Animation::getName() const
{
	return m_name;
}

bool Animation::hasEnded() const
{
	// basically checking if it's been played atleast once
	return (m_gameFrames > m_duration && m_currentKeyframe == 0) ? true : false;
}