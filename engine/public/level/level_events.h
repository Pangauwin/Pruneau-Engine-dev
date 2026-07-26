#pragma once

#include <entt/entt.hpp>

namespace Core
{
    
struct OnAwake {};
struct OnStart {};

struct OnUpdate {
    float dt;
};
struct OnLateUpdate {
    float dt;
};

struct OnEnable {};
struct OnDisable {};

struct OnRender {};
struct OnGUIRender {};

extern entt::dispatcher _dispatcher;
}