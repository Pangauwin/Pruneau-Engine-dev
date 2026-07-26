#pragma once

#include <string>

#include "core/log/log.h"
#include "entt/entity/fwd.hpp"
#include "entt/signal/fwd.hpp"

#include <entt/entt.hpp>
#include <vector>

namespace Core
{
using Entity = std::uint32_t;

struct Entity_info {
    std::string name = "Entity";
};

struct Parent {
    Entity entity{0};
};

struct Children {
    std::vector<Core::Entity> entities;
};

class Level
{

public:
	Level(const char* _name);
	~Level();

    // Forbids copy and affectation
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;

    // Allows move if needed
    Level(Level&&) = default;
    Level& operator=(Level&&) = delete;

    Entity CreateEntity(const char* _name, Entity _parent = 0);

    //TODO : later, move all the entity related functions to a proper entity class (maybe)

    template<typename T>
    void DeleteComponent(Entity _entity)
    {
        if(m_registry.all_of<T>(_entity))
            m_registry.erase<T>(_entity);
        else
            Core::LogMessageWarning("Component to erase not found !"); //TODO: Add component type name
    }

    template<typename T, typename... Args>
    void AddComponent(Entity _entity, Args&& ... args)
    {
        m_registry.emplace<T>(static_cast<entt::entity>(_entity), std::forward<Args>(args)...);
    }

    template<typename T>
    bool HasComponent(Entity _entity)
    {
        return m_registry.all_of<T>(_entity);
    }

    void SetParent(Entity _target, Entity _parent);
    void DestroyEntity(Entity _entity);

    bool HasParent(Entity _entity);
    Entity GetParent(Entity _entity);

    void OnAwake();
    void OnStart();
    void OnUpdate(float dt);
    void OnLateUpdate(float dt);
    void OnRender();
    void OnGUIRender();

    std::string name;

    entt::registry& GetRegistry()
    {
        return m_registry;
    }

    template <typename T>
    T& GetComponent(Entity _entity)
    {
        return m_registry.get<T>(static_cast<entt::entity>(_entity));
    }
    
private:
    void HandleEntityDestruction();

    entt::registry m_registry;

public:
    unsigned int camera_index = 0;
};
}