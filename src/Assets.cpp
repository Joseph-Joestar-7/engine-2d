#include "Assets.h"
#include <iostream>
#include <fstream>

Assets::Assets()
{
}

Assets::~Assets()
{
    std::cout << "Destroying Assets\n";
}

bool Assets::addTexture(const std::string& textureName, const std::string& path,
    bool smooth)
{
    sf::Texture texture;

    if (!texture.loadFromFile(path))
    {
        std::cerr << "Failed to load texture: " << textureName << " " <<
            path << std::endl;
        return false;
    }

    texture.setSmooth(smooth);
    m_textureMap[textureName] = texture;
    return true;
}

bool Assets::addTexture(const std::string& textureName, const std::string& path,
    int posX, int posY, int width, int height, bool smooth)
{
    sf::Texture texture;

    if (!texture.loadFromFile(path, sf::IntRect(posX, posY, width, height)))
    {
        std::cerr << "Failed to load texture: " << textureName << " " <<
            path << std::endl;
        return false;
    }

    texture.setSmooth(smooth);
    m_textureMap[textureName] = texture;

    return true;
}

bool Assets::addFont(const std::string& fontName, const std::string& path)
{
    sf::Font font;

    if (!font.loadFromFile(path))
    {
        std::cerr << "Failed to load font: " << fontName << " " << path <<
            std::endl;
        return false;
    }

    m_fontMap[fontName] = font;

    return true;
}

bool Assets::addAnimation(const std::string& animationName,
    const std::string& textureName,
    const size_t keyframesCount, const size_t duration)
{
    const auto& texture = getTexture(textureName);
    m_animationMap[animationName] = Animation(
        animationName, texture, keyframesCount,
        duration
    );
    return true;
}

const sf::Texture& Assets::getTexture(const std::string& textureName) const
{
    auto it = m_textureMap.find(textureName);
    if (it == m_textureMap.end())
    {
        throw std::runtime_error("Texture not found: " + textureName);
    }
    return it->second;
}

const Animation& Assets::getAnimation(const std::string& animationName) const
{
    auto it = m_animationMap.find(animationName);
    if (it == m_animationMap.end())
    {
        throw std::runtime_error("Animation not found: " + animationName);
    }
    return it->second;
}

const sf::Font& Assets::getFont(const std::string& fontName) const
{
    auto it = m_fontMap.find(fontName);
    if (it == m_fontMap.end())
    {
        throw std::runtime_error("Font not found: " + fontName);
    }
    return it->second;
}

void Assets::loadFromFile(const std::string& filePath)
{
    std::ifstream fin(filePath);
    if (!fin.is_open())
    {
        throw std::runtime_error("Failed to open asset file: " + filePath);
    }

    std::string token;

    while (fin >> token)
    {
        if (token == "Texture")
        {
            std::string name, path;
            int X, Y, width, height;
            bool smooth;

            if (!(fin >> name >> path >> X >> Y >> width >> height >> smooth))
            {
                throw std::runtime_error("Invalid Texture entry in " + filePath);
            }

            if (!addTexture(name, path, X, Y, width, height, smooth))
            {
                throw std::runtime_error("Failed to add texture: " + name);
            }
        }
        else if (token == "Animation")
        {
            std::string name, textureName;
            int keyframesCount, duration;

            if (!(fin >> name >> textureName >> keyframesCount >> duration))
            {
                throw std::runtime_error("Invalid Animation entry in " + filePath);
            }

            if (!addAnimation(name, textureName, keyframesCount, duration))
            {
                throw std::runtime_error("Failed to add animation: " + name);
            }
        }
        else if (token == "Font")
        {
            std::string name, path;

            if (!(fin >> name >> path))
            {
                throw std::runtime_error("Invalid Font entry in " + filePath);
            }

            if (!addFont(name, path))
            {
                throw std::runtime_error("Failed to add font: " + name);
            }
        }
        else
        {
            throw std::runtime_error("Bruh what is this: " + token);
        }
    }
}
