#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#include <entt/entt.hpp>
#include "core/log/log.h"
#include "entt/core/fwd.hpp"
#include "entt/core/type_info.hpp"
#include "entt/entity/fwd.hpp"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

namespace Core {

struct Component {};

struct ComponentInfo
{
    std::string name = "";

    std::uint16_t version = 0;

    std::function<bool(entt::registry& _reg, entt::entity _ent)> Has;

    std::function<void(rapidjson::Value&, rapidjson::Document::AllocatorType&, entt::registry&, entt::entity)> Save;
    std::function<bool(const rapidjson::Value&, entt::registry&, entt::entity)> Load;
    std::function<void(entt::registry&, entt::entity)> Add;
};

class ComponentRegistry
{
public:
    template<typename T>
    static void RegisterComponent(const std::string& name, 
        std::function<void(rapidjson::Value&, rapidjson::Document::AllocatorType&, const T&)> _save,
        std::function<bool(const rapidjson::Value&, T&)> _load)
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

        info.Has = [] (entt::registry& _reg, entt::entity _ent) 
        {
            return _reg.all_of<T>(_ent);
        };

        info.Save = [_save](rapidjson::Value& _val, rapidjson::Document::AllocatorType& _al, entt::registry& _reg, entt::entity _ent)
        {
            rapidjson::Value _comp(rapidjson::kObjectType);
            _save(_comp, _al, _reg.get<T>(_ent));

            std::string name = GetComponentInfo<T>()->name;

            _val.AddMember(rapidjson::Value(name.c_str(), _al).Move(), _comp, _al);
        };

        info.Load = [_load](const rapidjson::Value& _val,  entt::registry& _reg, entt::entity _ent)
        {
            return _load(_val, _reg.emplace_or_replace<T>(_ent));
        };

        info.Add = [] (entt::registry& _reg, entt::entity _ent)
        {
            _reg.emplace_or_replace<T>(_ent);
        };

        auto id = entt::type_hash<T>::value();

        m_component_registry.emplace(entt::type_hash<T>::value(), std::move(info));
        m_name_to_id.emplace(name, entt::type_hash<T>::value());
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

    static std::unordered_map<entt::id_type, ComponentInfo> m_component_registry; //Change this to directly string, ComponentInfo instead of having two unordered maps
    static std::unordered_map<std::string, entt::id_type> m_name_to_id;
};
}