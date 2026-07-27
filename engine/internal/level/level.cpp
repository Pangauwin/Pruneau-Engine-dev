#include "level/level.h"
#include "components/transform.h"
#include "core/log/log.h"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include <string>
#include <utility>
#include <vector>
#include <unordered_set>

#include "level/level_events.h"


static std::vector<Core::Entity> entities_to_destroy;

Core::Level::Level(const char* _name) : name(_name)
{
	
}

Core::Level::~Level()
{
	m_registry.clear();
}

Core::Entity Core::Level::CreateEntity(const char* _name, Entity _parent)
{
	auto ent =  m_registry.create();
	m_registry.emplace<Entity_info>(ent, (std::string)_name);
	m_registry.emplace<Transform>(ent);

	entt::entity parent = static_cast<entt::entity>(_parent);

	if(m_registry.valid(parent))
	{
		m_registry.emplace<Parent>(ent, _parent);
		Children& _children = m_registry.get_or_emplace<Children>(parent);
		_children.entities.emplace_back(entt::to_integral(ent));
	}

	return entt::to_integral(ent);
}

void Core::Level::DestroyEntity(Core::Entity _entity)
{
	entt::entity ent = static_cast<entt::entity>(_entity);

	if(!m_registry.valid(ent))
	{
		Core::LogMessageError("Could not destroy Entity " + std::to_string(_entity) + ": entity ID not found in registry");
		return;
	}

	if(m_registry.all_of<Children>(ent))
	{
		for(Core::Entity _ent : m_registry.get<Children>(ent).entities)
		{
			Core::Level::DestroyEntity(_ent);
		}
	}

	
	entities_to_destroy.push_back(_entity);
}

void Core::Level::SetParent(Entity _target, Entity _parent)
{
	if(m_registry.valid(static_cast<entt::entity>(_target)) && m_registry.valid(static_cast<entt::entity>(_parent)))
	{
		Parent& parent = m_registry.get_or_emplace<Parent>(static_cast<entt::entity>(_target));
		parent.entity = _parent;
		return;
	}
	else if(!m_registry.valid(static_cast<entt::entity>(_target)))
	{
		Core::LogMessageError("Failed to set parent: Entity " + std::to_string(_target) + " is not valid !");
	}
	else 
	{
		Core::LogMessageError("Failed to set parent: Entity " + std::to_string(_parent) + " is not valid !");
	}
}

void Core::Level::HandleEntityDestruction()
{
	std::vector<Core::Entity> to_destroy;
	std::swap(to_destroy, entities_to_destroy); // We don't want DestroyEntity to add element while we're destroying others

	std::unordered_set<Core::Entity> destroy_set(to_destroy.begin(), to_destroy.end());

	for(Core::Entity _entity: to_destroy)
	{
		m_registry.destroy(static_cast<entt::entity>(_entity));
	}

	entities_to_destroy.clear();
}

Core::Entity Core::Level::GetParent(Core::Entity _entity)
{
	if(!HasParent(_entity))
	{
		Core::LogMessageError("Entity " + std::to_string(_entity) + " has no parent!");
		return 0;
	}

	return m_registry.get<Parent>(static_cast<entt::entity>(_entity)).entity;
}

bool Core::Level::HasParent(Entity _entity)
{
	if(!m_registry.valid(static_cast<entt::entity>(_entity)))
	{
		Core::LogMessageError("Entity " + std::to_string(_entity) + " is not valid!");
		return false;	
	}

	return m_registry.all_of<Parent>(static_cast<entt::entity>(_entity));
}

void Core::Level::OnAwake()
{
	_dispatcher.trigger(Core::OnAwake{});
}

void Core::Level::OnStart()
{
	_dispatcher.trigger(Core::OnStart{});
}

void Core::Level::OnUpdate(float dt)
{
	_dispatcher.trigger(Core::OnUpdate{dt});
}

void Core::Level::OnLateUpdate(float dt)
{
	_dispatcher.trigger(Core::OnLateUpdate{dt});
	HandleEntityDestruction();
}

void Core::Level::OnRender()
{
	_dispatcher.trigger(Core::OnRender{});
}

void Core::Level::OnGUIRender()
{
	_dispatcher.trigger(Core::OnGUIRender{});
}