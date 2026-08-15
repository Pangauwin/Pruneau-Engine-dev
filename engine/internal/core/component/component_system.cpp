#include "core/component/component_system.h"

#include "entt/signal/fwd.hpp"
#include "level/level_events.h"

void Core::ComponentSystem::Connect()
{
    entt::dispatcher& dispatcher = Core::_dispatcher;

    dispatcher.sink<Core::OnAwake>().connect<&ComponentSystem::HandleAwake>(*this);
    dispatcher.sink<Core::OnStart>().connect<&ComponentSystem::HandleStart>(*this);
    dispatcher.sink<Core::OnUpdate>().connect<&ComponentSystem::HandleUpdate>(*this);
    dispatcher.sink<Core::OnLateUpdate>().connect<&ComponentSystem::HandleLateUpdate>(*this);
    dispatcher.sink<Core::OnEnable>().connect<&ComponentSystem::HandleEnable>(*this);
    dispatcher.sink<Core::OnDisable>().connect<&ComponentSystem::HandleDisable>(*this);
    dispatcher.sink<Core::OnRender>().connect<&ComponentSystem::HandleRender>(*this);
    dispatcher.sink<Core::OnGUIRender>().connect<&ComponentSystem::HandleGUIRender>(*this);
}