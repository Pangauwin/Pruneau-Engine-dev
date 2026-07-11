#include "level/level.h"
#include "core/entity.h"
#include <utility>
#include <vector>
#include <unordered_set>

static std::vector<Core::Entity*> entities_to_destroy;

Core::Level::Level(const char* _name) : name(_name)
{

}

Core::Level::~Level()
{
	for (auto& e : entities)
	{
		if(e->parent == nullptr)
		{
			DestroyEntity(e);
		}
	}
}

Core::Entity* Core::Level::CreateEntity(const char* _name, Entity* _parent)
{
	entities.push_back(new Entity(_name, _parent));
	return entities.back();
}

void Core::Level::DestroyEntity(Core::Entity* _entity)
{
	for(Core::Entity* _ent: _entity->GetChildren())
	{
		Core::Level::DestroyEntity(_ent);
	}

	_entity->OnDestroy();
	entities_to_destroy.push_back(_entity);
}

void Core::Level::OnAwake()
{
	for (auto& e : entities)
	{
		e->OnAwake();
	}
}

void Core::Level::OnStart()
{
	for (auto& e : entities)
	{
		e->OnStart();
	}
}

void Core::Level::OnUpdate(float dt)
{
	HandleEntityDestruction();

	for (auto& e : entities)
	{
		e->OnUpdate(dt);
	}
}

void Core::Level::OnRender()
{
	for (auto& e : entities)
	{
		e->OnRender();
	}
}

void Core::Level::OnGUIRender()
{
	for (auto& e : entities)
	{
		e->OnGUIRender();
	}
}

void Core::Level::HandleEntityDestruction()
{
	std::vector<Core::Entity*> to_destroy;
	std::swap(to_destroy, entities_to_destroy); // We don't want DestroyEntity to add element while we're destroying others

	std::unordered_set<Core::Entity*> destroy_set(to_destroy.begin(), to_destroy.end());

	for(Core::Entity* _entity: to_destroy)
	{
		if(_entity->parent && !destroy_set.count(_entity->parent))
		{
			std::vector<Entity*>* parent_vec = &_entity->parent->children;
			auto it = find(parent_vec->begin(), parent_vec->end(), _entity);
            if (it != parent_vec->end())
                parent_vec->erase(it);
		}

		auto it = find(entities.begin(), entities.end(), _entity);
        if (it != entities.end())
            entities.erase(it);

		delete _entity;
	}

	entities_to_destroy.clear();
}