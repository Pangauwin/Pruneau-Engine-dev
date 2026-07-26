#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <entt/entt.hpp>

#include "core/log/log.h"

namespace Core {

struct Component {};

struct ComponentInfo
{
    std::string name = "";
    size_t size = 0;

    std::uint16_t version = 0;
};

class ComponentRegistry
{
public:
    template<typename T>
    static void RegisterComponent(std::string name)
    {
        if(HasComponent<T>())
        {
            Core::LogMessageError("Component already registered!");
            return;
        }
        m_component_registry.emplace(entt::type_hash<T>::value(), 
            {
                .name=name, 
                .size=sizeof(T)
            });
    }

    template<typename T>
    static const ComponentInfo* GetComponentInfo()
    {
        auto it = m_component_registry.find(entt::type_hash<T>::value());
        return it != m_component_registry.end() ? &it->second : nullptr;
    }

    template<typename T>
    static bool HasComponent()
    {
        return m_component_registry.contains(entt::type_hash<T>::value());
    }

    static std::unordered_map<entt::id_type, ComponentInfo> m_component_registry;
};
}