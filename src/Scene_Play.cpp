#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "GameEngine.h"

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
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");   // Toggle drawing textures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing collision boxes
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");      // Toggle drawing Grid

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Arial"));

    loadLevel(levelPath);
}

void Scene_Play::loadLevel(const std::string& levelPath)
{
    m_entityManager = EntityManager();


}