#include "EntityManager.h"
#include <algorithm>
#include <iostream>

EntityManager::EntityManager()
{
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	vec.erase(
		std::remove_if(
			vec.begin(),
			vec.end(),
			[](auto it) { return !it->isActive(); }
		),
		vec.end()
	);
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	auto it = m_entitiesMap.find(tag);
	if (it == m_entitiesMap.end())
	{
		throw std::runtime_error("Tag '" + tag + "' does not exist in EntityManager");
	}
	return it->second;
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag)); //since i made the entity class priv, can't really do make shared ptr
	std::cout << "added " << tag << " \n";
	m_entitiesToAdd.push_back(entity);
	return entity;
}

void EntityManager::update()
{
	for (const auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entitiesMap[e->tag()].push_back(e);
	}

	m_entitiesToAdd.clear();

	removeDeadEntities(m_entities);

	for (auto& [tag, entityVec_] : m_entitiesMap)
	{
		removeDeadEntities(entityVec_);
	}
}


