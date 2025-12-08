#include "GameEngine.h"
#include "Action.h"
#include "Scene_Menu.h"
#include "fstream"

GameEngine::GameEngine(const std::string& filePath)
{
	init(filePath);
}

void GameEngine::init(const std::string& filePath)
{
	std::ifstream fin(filePath);
	std::string token;
	int width=1080, height=1080, frameRate=60;
	bool fullScreenMode =true;

	while (fin.good())
	{
		fin >> token >> width >> height >> frameRate >> fullScreenMode;
	}

	if (fullScreenMode)
	{
		m_window.create(sf::VideoMode(width, height), "Custom Game Engine", sf::Style::Fullscreen);
	}
	else
	{
		m_window.create(sf::VideoMode(width, height), "Custom Game Engine");
	}

	m_window.setFramerateLimit(frameRate); 

	m_assets.loadFromFile("bin/assets.txt");

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

void GameEngine::update()
{
	sUserInput();
	currentScene()->update();
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_SceneMap[m_currentScene];
}

void GameEngine::sUserInput()
{
	sf::Event event;

	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}
		else if (event.type == sf::Event::KeyPressed ||
			event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) continue;

			const std::string& actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			currentScene()->sDoAction(
				Action(
					currentScene()->getActionMap().at(event.key.code),
					actionType
				)
			);
		}
	}
}

bool GameEngine::isRunning() const
{
	return m_running && m_window.isOpen();
}

const Assets& GameEngine::assets() const
{
	return m_assets;
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

void GameEngine::run()
{
	while (isRunning())
	{
		update();
	}

	m_window.close();
}

void GameEngine::quit()
{
	m_running = false;
}

void GameEngine::changeScene(const std::string& sceneName,
	std::shared_ptr<Scene> scene)
{
	m_SceneMap[sceneName] = scene;
	m_currentScene = sceneName;
}

