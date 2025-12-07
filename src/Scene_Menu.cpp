#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Assets.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Enter, "SELECT");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "Some Platformer Lol";
	m_menuStrings.push_back("Level 1");
	m_menuStrings.push_back("Level 2");
	m_menuStrings.push_back("Level 3");

	m_levelPaths.push_back("level1.txt");
	m_levelPaths.push_back("level2.txt");
	m_levelPaths.push_back("level3.txt");
	
	m_menuText.setFont(m_game->assets().getFont("Mario"));
	m_menuText.setCharacterSize(64);
}

void Scene_Menu::update()
{
	sRender();
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "SELECT")
		{
			
		}
		else if (action.name() == "UP")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex > 0)
				? --m_selectedMenuIndex
				: m_menuStrings.size() - 1;
		}
		else if (action.name() == "DOWN")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) %
				m_menuStrings.size();
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Menu::sRender()
{
	auto& window = m_game->window();

	window.clear(sf::Color::Blue); 

	m_menuText.setString(m_title);
	m_menuText.setFillColor(sf::Color::White);

	const float posX = window.getSize().x / 2.0f -
		m_menuText.getLocalBounds().width / 2.0f;
	const float posY = 10;
	m_menuText.setPosition(posX, posY);
	window.draw(m_menuText);

	for (int stringIndex = 0; stringIndex < m_menuStrings.size(); stringIndex++)
	{
		const float posX = 20;
		const float posY = 150 + stringIndex * 100;
		m_menuText.setString(m_menuStrings[stringIndex]);
		m_menuText.setPosition(posX, posY);

		if (m_selectedMenuIndex == stringIndex)
		{
			m_menuText.setFillColor(sf::Color::Black); 
		}
		else
		{
			m_menuText.setFillColor(sf::Color::White); 
		}

		window.draw(m_menuText);
	}

	window.display();
}