#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>
#include "Physics.h"

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
    registerAction(sf::Keyboard::F, "DEBUG");

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
    
    //Will figure the sfml to regular math mapping later for now just gotta keep the low as y =16 in the level files hehe

    auto& anim = entity->getComponent<CAnimation>().animation;
    float w = (float)anim.getSize().x;
    float h = (float)anim.getSize().y;

    float px = gridX * m_gridSize.x;
    float py = gridY * m_gridSize.y;

    px += w * 0.5f;
    py += h * 0.5f;

    return sf::Vector2f(px, py);

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
    std::string tag;
    float gx, gy, sx, sy;
    std::string type;
    while (file >> type)
    {
        if (type == "TileL" || type == "TileM" || type == "TileR" || type == "TileQ" || type == "TileB")
        {
            file >> tag >> gx >> gy;

            auto tile = m_entityManager.addEntity("Tile");

            tile->addComponent<CAnimation>(m_game->assets().getAnimation(type), true);
            tile->addComponent<CTransform>();
            tile->getComponent<CTransform>().pos = gridToMidPixel(gx, gy, tile);
            const auto& spriteSize = tile->getComponent<CAnimation>().animation.getSize();

            sx = 64/ spriteSize.x;
            sy = 64 / spriteSize.y;

            tile->getComponent<CTransform>().scale = sf::Vector2f(sx, sy);
            tile->addComponent<CBoundingBox>(m_gridSize);
        }
        else if (type == "Dec")
        {
            file >> tag >> gx >> gy>>sx>>sy;

            auto e = m_entityManager.addEntity(tag);

            e->addComponent<CAnimation>(m_game->assets().getAnimation(tag), true);
            
            e->addComponent<CTransform>();
            e->getComponent<CTransform>().pos = gridToMidPixel(gx, gy, e);

            e->getComponent<CTransform>().scale = sf::Vector2f(sx,sy);
        }
        else
        {
            spawnPlayer(file);
        }
    }
}

void Scene_Play::spawnPlayer(std::ifstream& file)
{
    float gx, gy;
    float boxW, boxH;
    float leftRightSpeed, jumpSpeed, maxSpeed, gravity;
    std::string bulletAnim;

    file >> gx >> gy
        >> boxW >> boxH
        >> leftRightSpeed >> jumpSpeed
        >> maxSpeed >> gravity
        >> bulletAnim;

    m_player = m_entityManager.addEntity("player");
    sf::Vector2f box =  { boxW, boxH };
    m_player->addComponent<CTransform>(gridToMidPixel(gx, gy, m_player));
    m_player->addComponent<CBoundingBox>(box);

    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("PlayerIdle"), true);
    const auto& spriteSize = m_player->getComponent<CAnimation>().animation.getSize();

    float sx = 64 / spriteSize.x;
    float sy = 64 / spriteSize.y;


    m_player->getComponent<CTransform>().scale = { sx,sy };
    m_player->addComponent<CInput>();
    m_player->addComponent<CState>();
    m_player->addComponent<CGravity>(gravity);
    

}

void Scene_Play::sMovement()
{
    sf::Vector2f playerVelocity(0, 0);

    auto& input = m_player->getComponent<CInput>();
    auto& playerState = m_player->getComponent<CState>().state;
    auto& playerTransform = m_player->getComponent<CTransform>();

    if (playerState != "jump")
    {
        if (input.up)
        {
            if (playerState == "run" || playerState == "idle")
            {
                playerState = "jump";
            }

            playerVelocity.x = playerTransform.velocity.x;
            playerVelocity.y -= 15;
        }

        else if (input.right)
        {
            playerVelocity.x += 5;
            playerState = "run";
            playerTransform.scale.x = abs(playerTransform.scale.x);
        }
        else if (input.left)
        {
            playerVelocity.x -= 5;
            playerState = "run";
            playerTransform.scale.x = -abs(playerTransform.scale.x);
        }
        else if (!input.right && playerState == "run")
        {
            playerState = "idle";
            playerTransform.scale.x = abs(playerTransform.scale.x);
        }
        else if (!input.left && playerState == "run")
        {
            playerState = "idle";
            playerTransform.scale.x = -abs(playerTransform.scale.x);
        }

        m_player->getComponent<CTransform>().velocity = playerVelocity;
    }

    for (auto e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CGravity>())
        {
            e->getComponent<CTransform>().velocity.y +=
                e->getComponent<CGravity>().gravity;
        }

        e->getComponent<CTransform>().pos +=
            e->getComponent<CTransform>().velocity;
    }
}

void Scene_Play::sAnimation()
{
}

void Scene_Play::sCollision()
{
    auto tiles = m_entityManager.getEntities("Tile");
    
    for (auto t : tiles)
    {
        auto overlap = Physics::GetOverlap(m_player, t);

        if (overlap.x > 0 && overlap.y > 0)
        {
            resolveCollision(m_player, t);
        }
    }
}

void Scene_Play::resolveCollision(std::shared_ptr<Entity> player, std::shared_ptr<Entity> tile)
{
    auto overlap = Physics::GetOverlap(player, tile);
    auto prevOverlap = Physics::GetPreviousOverlap(player, tile);

    auto& pTrans = player->getComponent<CTransform>();
    //std::cout << overlap.x << " " << overlap.y << " " << prevOverlap.x << " " << prevOverlap.y << "\n";
    if (prevOverlap.y > 0 && prevOverlap.x <= 0)
    { // side wise collision
        if (pTrans.pos.x < tile->getComponent<CTransform>().pos.x)
            pTrans.pos.x -= overlap.x; 
        else
            pTrans.pos.x += overlap.x; 

        pTrans.velocity.x = 0;
    }

    else if (prevOverlap.x > 0 )
    {
        //all these because sfml y axis is inverted and i am too tired to do the math of making it normal
        if (pTrans.pos.y < tile->getComponent<CTransform>().pos.y)
        {
            std::cout << "This case " << "\n";
            pTrans.pos.y -= overlap.y;
            pTrans.velocity.y = 0;
            player->getComponent<CState>().state = "grounded";
        }
        else
        {
            pTrans.pos.y += overlap.y;
            pTrans.velocity.y = 0;
        }
    }
}

void Scene_Play::setAnimation(std::shared_ptr<Entity> entity, const std::string& animationName, bool repeat)
{
}

void Scene_Play::drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
}



void Scene_Play::update()
{
    if (!m_paused)
    {
        m_entityManager.update();
        sMovement();
        sCollision();
    }
    sRender();
}

void Scene_Play::onEnd()
{
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Play::sRender()
{
    auto& window = m_game->window();

    if (!m_paused)
    {
        window.clear(sf::Color(100, 100, 255));
    }
    else
    {
        window.clear(sf::Color(50, 50, 150));
    }

    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& pos = e->getComponent<CTransform>().pos;
            auto& scale = e->getComponent<CTransform>().scale;
            auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
            e->getComponent<CAnimation>().animation.update();
            sprite.setPosition(pos.x, pos.y);
            sprite.setScale(scale.x, scale.y);

            window.draw(sprite);
        }
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

    

    window.display();
}

void Scene_Play::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "TOGGLE_COLLISION")
            m_drawCollision = !m_drawCollision;
        else if (action.name() == "DEBUG")
            flag = !flag;
        else if (action.name() == "EXIT")
            onEnd();
        else if (action.name() == "JUMP")
        {
            m_player->getComponent<CInput>().up = true;
            std::cout << m_player->getComponent<CState>().state << "\n";
        }
        else if (action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = true;
        }
        else if (action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = true;
        }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "JUMP")
        {
            m_player->getComponent<CInput>().up = false;
        }
        else if (action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = false;
        }
        else if (action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = false;
        }
    }
}
