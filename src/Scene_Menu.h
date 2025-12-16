#pragma once
#include "Scene.h"


class Scene_Menu : public Scene
{
private:
	std::string              m_title;
	std::vector<std::string> m_menuStrings;
	std::vector<std::string> m_levelPaths;
	sf::Text                 m_menuText;
	size_t                   m_selectedMenuIndex = 0;

	void init();

	void onEnd()                         override;
	void update()                        override;
	void sRender()                       override;
	void sDoAction(const Action& action) override;
	void sCamera()                       override;

public:
	Scene_Menu(GameEngine* gameEngine);
};

