#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#include <entt/entt.hpp>

#include "cereal/archives/json.hpp"
#include "core/log/log.h"
#include "entt/core/fwd.hpp"
#include "entt/entity/fwd.hpp"

namespace Core {

using SaveArchive = cereal::JSONOutputArchive;
using LoadArchive = cereal::JSONInputArchive;

struct Component {};

struct ComponentInfo
{
    std::string name = "";
    size_t size = 0;

    std::uint16_t version = 0;

    std::function<bool(entt::registry& _reg, entt::entity _ent)> Has;

    std::function<void(SaveArchive&, entt::registry&, entt::entity)> Save;
    std::function<void(LoadArchive&, entt::registry&, entt::entity)> Load;
};

class ComponentRegistry
{
public:
    template<typename T>
    static void RegisterComponent(const std::string& name, 
        std::function<void(SaveArchive&, const T&)> _save,
        std::function<void(LoadArchive&, T&)> _load)
    {

        if(HasComponent<T>())
        {
            Core::LogMessageError("Component registration failed: Component already registered !");
            return;
        }

        if(HasComponentName(name))
        {
            Core::LogMessageError("Component registration failed: Another component already use this name !");
            return;
        }

        ComponentInfo info;

        info.name = std::move(name);
        info.size = sizeof(T);

        info.Has = [] (entt::registry& _reg, entt::entity _ent) 
        {
            return _reg.all_of<T>(_ent);
        };

        info.Save = [_save](cereal::JSONOutputArchive& ar, entt::registry& _reg, entt::entity _ent)
        {
            _save(ar, _reg.get<T>(_ent));
        };

        info.Load = [_load](cereal::JSONInputArchive& ar, entt::registry& _reg, entt::entity _ent)
        {
            _load(ar, _reg.emplace<T>(_ent));
        };

        auto id = entt::type_hash<T>::value();

        m_component_registry.emplace(entt::type_hash<T>::value(), std::move(info));
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

    static bool HasComponentName(std::string _name)
    {
        return m_name_to_id.contains(_name);
    }

    static std::unordered_map<entt::id_type, ComponentInfo> m_component_registry;
    static std::unordered_map<std::string, entt::id_type> m_name_to_id;
};
}