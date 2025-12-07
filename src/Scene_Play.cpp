#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
{
    init(levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
    registerAction(sf::Keyboard::W, "JUMP");
    registerAction(sf::Keyboard::Up, "JUMP");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Right, "RIGHT");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::Left, "LEFT");
    registerAction(sf::Keyboard::Escape, "EXIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");   
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); 
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");      

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Mario"));

    loadLevel(levelPath);
}

sf::Vector2f Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    //TODO: This function takes in a grid x y and an entity
    // Return a vec2 indicating where the center pos of the entity should be
    // Must use Entity's animation size to position it correctly
    // The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    // The bottom left corner of the Animation should align with the bottom left of the grid cell
    
    return sf::Vector2f();
}



void Scene_Play::loadLevel(const std::string& levelPath)
{
    m_entityManager = EntityManager();

    std::ifstream file(levelPath);
    if (!file.is_open())
    {
        std::cerr << "Cannot open level file: " << levelPath << "\n";
        return;
    }

    std::string type;
    while (file >> type)
    {
        if (type == "TileL" || type == "TileM" || type == "TileR")
        {
            std::string tag;
            float gx, gy;

            file >> tag >> gx >> gy;

            auto tile = m_entityManager.addEntity("tile");
        }

    }
}

void Scene_Play::update()
{
    if (!m_paused)
    {
        m_entityManager.update();
    }
    sRender();
}

void Scene_Play::sRender()
{
    // TODO: remove this testing code
    auto& window = m_game->window();

    if (!m_paused)
    {
        window.clear(sf::Color(100, 100, 255));
    }
    else
    {
        window.clear(sf::Color(50, 50, 150));
    }

    if (m_drawGrid)
    {
    }
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x, box.size.y));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                window.draw(rect);
            }
        }
    }

    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& pos = e->getComponent<CTransform>().pos;
            auto& scale = e->getComponent<CTransform>().scale;
            auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
            sprite.setPosition(pos.x, pos.y);
            sprite.setScale(scale.x, scale.y);

            window.draw(sprite);
        }
    }

    window.display();
}