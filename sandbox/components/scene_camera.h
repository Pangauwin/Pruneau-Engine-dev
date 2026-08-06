#pragma once

#include "core/component/component_system.h"
#include "core/component/component.h"
#include "rapidjson/document.h"

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
public:
    void Register() override {
        Core::ComponentRegistry::RegisterComponent<SceneCamera>(
            "Editor Camera", 
            [](rapidjson::Value& _val, rapidjson::Document::AllocatorType& _al, const SceneCamera& _t) {
                _val.AddMember("camera_speed", _t.camera_speed, _al);
                _val.AddMember("camera_sensitivity", _t.camera_sensitivity, _al);
                _val.AddMember("yaw", _t.yaw, _al);
                _val.AddMember("pitch", _t.pitch, _al);
            },
            [](const rapidjson::Value& _val, SceneCamera& _t) {
                if(!(_val.HasMember("camera_speed")
                    && _val.HasMember("camera_sensitivity")
                    && _val.HasMember("yaw")
                    && _val.HasMember("pitch"))) return false;

                _t.pitch = _val["pitch"].GetFloat();
                _t.yaw = _val["yaw"].GetFloat();
                _t.camera_sensitivity = _val["camera_sensitivity"].GetFloat();
                _t.camera_speed = _val["camera_speed"].GetFloat();

                return true;
            });
    }
};

}