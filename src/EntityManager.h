#pragma once
#include "Entity.h"
#include <map>
#include <memory>
#include <vector>

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;     

class EntityManager
{
private:
	EntityVec m_entities;
	EntityVec m_entitiesToAdd;
	EntityMap m_entitiesMap;
	uint64_t  m_totalEntities = 0;

	void removeDeadEntities(EntityVec& vec);

public:
	EntityManager();

	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);

	std::shared_ptr<Entity> addEntity(const std::string& tag);
	void                    update();
};

