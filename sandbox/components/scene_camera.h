#pragma once

#include "core/component/component_system.h"
#include "core/component/component.h"

namespace Sandbox
{

struct SceneCamera : public Core::Component
{
    float camera_speed = 4.0f;
    float camera_sensitivity = 0.15f;

    float yaw = 0.0f;
    float pitch = 0.0f;

    //TODO: implement a focus scene camera system
};

class SceneCameraSystem : public Core::ComponentSystem
{
    void OnUpdate() override;
    void Register() override {
        
    }
};

}