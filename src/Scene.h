#pragma once
#include "Action.h"
#include "Entity.h"


class GameEngine; //Forward declaration 

using ActionMap = std::map<int, std::string>;

class Scene
{
protected:
	GameEngine* m_game = nullptr;
	uint64_t      m_currentFrame = 0;
	bool          m_hasEnded = false;
	bool          m_paused = false;
	ActionMap     m_actionMap;

public:
	Scene();
	Scene(GameEngine* game);

	virtual void update() = 0;
	virtual void onEnd() = 0;
	virtual void sRender() = 0;
	virtual void sDoAction(const Action& action) = 0;

	const ActionMap& getActionMap() const;

	void registerAction(int inputKey, const std::string& actionName);
	void setPaused(bool paused);
};

