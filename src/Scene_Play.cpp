#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>
#include "Physics.h"
#include <cassert>

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
    registerAction(sf::Keyboard::Space, "GUN");
    registerAction(sf::Keyboard::Escape, "EXIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");   
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); 
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");  
    registerAction(sf::Keyboard::F, "DEBUG");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Mario"));

    m_view = m_game->window().getDefaultView();
    m_cameraPos = m_view.getCenter();

    loadLevel(levelPath);

    m_entityManager.update(); // just do it once so that the tile entities are commited 
    m_levelWidth = calcLevelWidth(); // to dynamically calc the level width
    std::cout << m_levelWidth << "\n";
}

sf::Vector2f Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    //TODO: This function takes in a grid x y and an entity
    // Return a vec2 indicating where the center pos of the entity should be
    // Must use Entity's animation size to position it correctly
    // The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    // The bottom left corner of the Animation should align with the bottom left of the grid cell
    
    //Will figure the sfml to regular math mapping later for now just gotta keep the low as y =16 in the level files hehe

    assert(entity->hasComponent<CAnimation>() && "gridToMidPixel requires CAnimation");

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
    // CAMERA CONFIG
    m_cameraType = CameraType::FollowX; // For now just set the camera through code

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

            if (type == "TileQ")
                tile->addComponent<CGameplayTags>("Unused");
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
        else if (type == "Gun")
        {
            file >> m_gunConfig.CX >> m_gunConfig.CY >> m_gunConfig.SPEED;
        }
        else
        {
            file >> m_playerConfig.X >> m_playerConfig.Y
                >> m_playerConfig.CX >> m_playerConfig.CY
                >> m_playerConfig.SPEED >> m_playerConfig.JUMP
                >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY;
            spawnPlayer();
        }
    }
}

float Scene_Play::calcLevelWidth() 
{
    float maxRightEdge = 0.0f;

    auto& tiles = m_entityManager.getEntities("Tile");

    for (const auto& tile : tiles)
    {
        const auto& transform = tile->getComponent<CTransform>();
        const auto& box = tile->getComponent<CBoundingBox>();

        const float rightEdge = transform.pos.x + box.halfSize.x;
       
        if (rightEdge > maxRightEdge)
        {
            maxRightEdge = rightEdge;
        }
    }

    if (maxRightEdge <= 0.0f)
    {
        return m_view.getSize().x;
    }

    return maxRightEdge;
}


void Scene_Play::spawnPlayer()
{
    
    m_player = m_entityManager.addEntity("player");
    sf::Vector2f box =  { m_playerConfig.CX,m_playerConfig.CY };
    
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("PlayerIdle"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X,m_playerConfig.Y, m_player));
    m_player->addComponent<CBoundingBox>(box);
    
    const auto& spriteSize = m_player->getComponent<CAnimation>().animation.getSize();

    float sx = 64 / spriteSize.x;
    float sy = 64 / spriteSize.y;


    m_player->getComponent<CTransform>().scale = { sx,sy };
    m_player->addComponent<CInput>();
    m_player->addComponent<CGameplayTags>("idle"); //0
    m_player->getComponent<CGameplayTags>().gameplayTags.push_back("false"); //1
    m_player->getComponent<CGameplayTags>().gameplayTags.push_back("nogun"); //2
    m_player->getComponent<CGameplayTags>().gameplayTags.push_back("noshot"); //3
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    

}

void Scene_Play::spawnBullet()
{
    auto b = m_entityManager.addEntity("bullet");
    float multiplier = m_player->getComponent<CTransform>().scale.x > 0 ? 1 : -1;
    sf::Vector2f pos = { m_player->getComponent<CTransform>().pos.x + (multiplier* (m_gridSize.x/2 + 5)), m_player->getComponent<CTransform>().pos.y };
    b->addComponent<CTransform>(pos);
    b->getComponent<CTransform>().velocity = { multiplier * m_gunConfig.SPEED,0 };

    b->addComponent<CAnimation>(m_game->assets().getAnimation("Bullet"), false);

    const auto& spriteSize = b->getComponent<CAnimation>().animation.getSize();

    float sx = 64 / spriteSize.x;
    float sy = 64 / spriteSize.y;
    b->getComponent<CTransform>().scale = { sx,sy };

    sf::Vector2f box = { m_gunConfig.CX,m_gunConfig.CY };
    b->addComponent<CBoundingBox>(box);

} 

void Scene_Play::spawnCoin(sf::Vector2f& pos)
{
    auto coin = m_entityManager.addEntity("Coin");
    coin->addComponent<CTransform>();
    coin->getComponent<CTransform>().pos = pos;

    coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);

    const auto& spriteSize = coin->getComponent<CAnimation>().animation.getSize();

    float sx = 64 / spriteSize.x;
    float sy = 64 / spriteSize.y;
    coin->getComponent<CTransform>().scale = { sx,sy };

    coin->addComponent<CBoundingBox>(m_gridSize);

}

void Scene_Play::sMovement()
{
    sf::Vector2f playerVelocity(0, 0);

    auto& input = m_player->getComponent<CInput>();
    auto& playerTags = m_player->getComponent<CGameplayTags>().gameplayTags;
    std::string& isG = m_player->getComponent<CGameplayTags>().gameplayTags[1];
    auto& playerTransform = m_player->getComponent<CTransform>();

    //basically setting it back to idle if you've finished jump already
    //it ain't perfect but it works so all is well
    if (isG == "true" && playerTags[0] == "jump")
    {
        playerTags[0] = "idle";
    }

    if (isG == "true")
    {
        if (input.up)
        {
            if (playerTags[0] == "run" || playerTags[0] == "idle")
            {
                playerTags[0] = "jump";
                isG = "false";
            }

            playerVelocity.x = playerTransform.velocity.x;
            playerVelocity.y += m_playerConfig.JUMP;
        }

        else if (input.right)
        {
            playerVelocity.x += m_playerConfig.SPEED;
            playerTags[0] = "run";
            playerTransform.scale.x = abs(playerTransform.scale.x);
            
        }
        else if (input.left)
        {
            playerVelocity.x -= m_playerConfig.SPEED;
            playerTags[0] = "run";
            playerTransform.scale.x = -abs(playerTransform.scale.x);
        }
        else if (!input.right && playerTags[0] == "run")
        {
            
            playerTags[0] = "idle";
            playerTransform.scale.x = abs(playerTransform.scale.x);
        }
        else if (!input.left && playerTags[0] == "run")
        {
            
            playerTags[0] = "idle";
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
    const auto& playerTags = m_player->getComponent<CGameplayTags>().gameplayTags;

    if (playerTags[2] == "gun")
    {
        if (playerTags[0] == "run")
        {
            setAnimation(m_player, "PlayerRunGun", true);
        }
        else if (playerTags[0] == "idle")
        {
            setAnimation(m_player, "PlayerIdleGun", true);
        }
        else if (playerTags[0] == "jump")
        {
            setAnimation(m_player, "PlayerJumpGun", true);
        }
    }
    else
    {
        if (playerTags[0] == "run")
        {
            setAnimation(m_player, "PlayerRun", true);
        }
        else if (playerTags[0] == "idle")
        {
            setAnimation(m_player, "PlayerIdle", true);
        }
        else if (playerTags[0] == "jump")
        {
            setAnimation(m_player, "PlayerJump", true);
        }
    }
    

    m_player->getComponent<CAnimation>().animation.update();
}

void Scene_Play::setAnimation(std::shared_ptr<Entity> entity, const std::string& animationName, bool repeat)
{
    if (entity->getComponent<CAnimation>().animation.getName() != animationName) {
        entity->getComponent<CAnimation>() = CAnimation(m_game->assets().getAnimation(animationName), repeat); 
    }
}

void Scene_Play::sCollision()
{
    auto collidingEntities = m_entityManager.getEntities();

    //I am setting the isGrounded false so that I can check for each tick, if I'm in the air
    //Since it's already false before resolve collision is called -> if no block overlapped with us, it retains value
    m_player->getComponent<CGameplayTags>().gameplayTags[1] = "false";

    for (auto t : collidingEntities)
    {
        if (!t->hasComponent<CBoundingBox>() || t == m_player)
            continue;

        auto overlap = Physics::GetOverlap(m_player, t);

        if (overlap.x > 0 && overlap.y > 0)
        {
            if (t->getComponent<CAnimation>().animation.getName() == "Coin")
            {
                m_player->getComponent<CScore>().coins++;
                t->destroy();
                break;
            }
            resolveCollision(m_player, t);
        }
    }

    auto& bullets = m_entityManager.getEntities("bullet");
   
    for (auto b : bullets)
    {
        for (auto t : collidingEntities)
        {
            if (!t->hasComponent<CBoundingBox>() || t == b || t->getComponent<CAnimation>().animation.getName() == "Coin")
                continue;

            auto overlap = Physics::GetOverlap(b, t);
            if (overlap.x > 0 && overlap.y > 0)
            {
                // Destroy bullet on impact
                b->destroy();

                if (t->getComponent<CAnimation>().animation.getName() == "TileB")
                {
                    setAnimation(t, "Explode", false);
                    t->removeComponent<CBoundingBox>();
                    t->addComponent<CLifespan>(t->getComponent<CAnimation>().animation.getAnimDuration());
                }

                break;
            }
        }
    }
}

void Scene_Play::sLifeSpan()
{
    for (auto e : m_entityManager.getEntities())
    {
        if (!e->hasComponent<CLifespan>())
            continue;

        auto& life = e->getComponent<CLifespan>();
        life.remaining--;

        if (life.remaining <= 0)
        {
            e->destroy();   
        }
    }
}

void Scene_Play::resolveCollision(std::shared_ptr<Entity> player, std::shared_ptr<Entity> tile)
{
    auto overlap = Physics::GetOverlap(player, tile);
    auto prevOverlap = Physics::GetPreviousOverlap(player, tile);

    auto& pTrans = player->getComponent<CTransform>();
    
    if (prevOverlap.x > 0 )
    {
        //all these because sfml y axis is inverted and i am too tired to do the math of making it normal
        if (pTrans.pos.y < tile->getComponent<CTransform>().pos.y)
        {
            pTrans.pos.y -= overlap.y;
            pTrans.velocity.y = 0;
            player->getComponent<CGameplayTags>().gameplayTags[1] = "true";
        }
        else
        {
            pTrans.pos.y += overlap.y;
            pTrans.velocity.y = 0;
            std::string tileName = tile->getComponent<CAnimation>().animation.getName();
            if (tileName == "TileB" || tileName == "TileQ")
            {
                handleSpecialBlock(tile,tileName);
            }
        }
    }
    if (prevOverlap.y > 0 && prevOverlap.x<=0)
    { // side wise collision
        std::cout << pTrans.pos.x << "\n";
        if (pTrans.pos.x < tile->getComponent<CTransform>().pos.x)
            pTrans.pos.x -= overlap.x;
        else
            pTrans.pos.x += overlap.x;
        std::cout << pTrans.pos.x << "\n";
        pTrans.velocity.x = 0;
    }
    
}

void Scene_Play::handleSpecialBlock(std::shared_ptr<Entity> tile,std::string tileName)
{
    if (tileName == "TileB")
    {
        setAnimation(tile, "Explode", false);
        float totalFrames = tile->getComponent<CAnimation>().animation.getAnimDuration();
        tile->removeComponent<CBoundingBox>();
        tile->addComponent<CLifespan>(totalFrames);
    }
    else if (tileName == "TileQ")
    {
        if (tile->getComponent<CGameplayTags>().gameplayTags[0] == "Used")
            return;

        tile->getComponent<CGameplayTags>().gameplayTags[0] = "Used";
        tile->getComponent<CAnimation>().repeat = false;
        sf::Vector2f pos = { tile->getComponent<CTransform>().pos.x,tile->getComponent<CTransform>().pos.y - m_gridSize.y }; // bruh i swear i wasn't using 64 as magic number here
        spawnCoin(pos);
    }
}




void Scene_Play::drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
}



void Scene_Play::update()
{
    if (!m_paused)
    {
        m_entityManager.update();
        sCamera();
        sMovement();
        sCollision();
        sLifeSpan();
        sAnimation(); 
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

    //Render World Textures below:
    window.setView(m_view);

    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& pos = e->getComponent<CTransform>().pos;
            auto& scale = e->getComponent<CTransform>().scale;
            auto& sprite = e->getComponent<CAnimation>().animation.getSprite();

            if(e->getComponent<CAnimation>().repeat || e->getComponent<CAnimation>().animation.hasEnded()== false)
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

    //Render UI below:

    window.setView(window.getDefaultView());

    m_coinText.setFont(m_game->assets().getFont("Mario"));
    m_coinText.setCharacterSize(32);

    m_coinText.setString("Coins = " + std::to_string(m_player->getComponent<CScore>().coins));
    m_coinText.setFillColor(sf::Color::White);

    const float paddingX = 20;
    const float paddingY = 5;
    const float posX = window.getSize().x - m_coinText.getLocalBounds().width - paddingX;
    const float posY = 10 + paddingY;
    m_coinText.setPosition(posX, posY);

    window.draw(m_coinText);

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
        }
        else if (action.name() == "LEFT")
        {
            m_player->getComponent<CInput>().left = true;
        }
        else if (action.name() == "RIGHT")
        {
            m_player->getComponent<CInput>().right = true;
        }
        else if (action.name() == "GUN")
        {
            if (m_player->getComponent<CGameplayTags>().gameplayTags[3] == "shot")
                return;
            m_player->getComponent<CGameplayTags>().gameplayTags[3] = "shot";
            m_player->getComponent<CGameplayTags>().gameplayTags[2] = "gun";
            spawnBullet();
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
        else if (action.name() == "GUN")
        {
            m_player->getComponent<CGameplayTags>().gameplayTags[3] = "noshot";
            m_player->getComponent<CGameplayTags>().gameplayTags[2] = "nogun";
        }
    }
}

void Scene_Play::sCamera()
{
    if (!m_player)
        return;

    const auto& playerPos = m_player->getComponent<CTransform>().pos;
    auto& window = m_game->window();

    sf::Vector2f target = m_cameraPos;

    const float halfW = m_view.getSize().x * 0.5f;
    const float halfH = m_view.getSize().y * 0.5f;

    if (m_cameraType == CameraType::FollowX)
    {
        target.x = playerPos.x;

        target.x = std::max(halfW, target.x);
        target.x = std::min(m_levelWidth - halfW, target.x);

        target.y = halfH;
    }
    else if (m_cameraType == CameraType::FollowXY)
    {
        //TODO
    }
    else
    {
        //TODO
    }

    m_cameraPos += (target - m_cameraPos) * m_cameraSpeed;

    m_view.setCenter(m_cameraPos);
    window.setView(m_view);
}

