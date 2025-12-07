#pragma once
#include "Assets.h"
#include "Scene.h"

#include <map>
#include <memory>

class Scene;

using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

class GameEngine
{
protected:
	sf::RenderWindow m_window;
	std::string      m_currentScene;
	Assets           m_assets;
	SceneMap         m_SceneMap;
	bool             m_running = true;

	void                   init(const std::string& filePath);
	void                   update();
	std::shared_ptr<Scene> currentScene();
	void                   sUserInput();

public:
	GameEngine(const std::string& filePath);

	bool              isRunning() const;
	const Assets& assets()    const;
	sf::RenderWindow& window();

	void run();
	void quit();
	void changeScene(const std::string& sceneName,
		std::shared_ptr<Scene> scene);
};