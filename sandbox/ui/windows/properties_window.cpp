#include "properties_window.h"

#include <imgui.h>
#include "ImGuizmo.h"
#include "components/camera.h"
#include "components/transform.h"
#include "core/application.h"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include "layer/sandbox_layer.h"

#include "level/level.h"
#include "level/level_manager.h"
#include "renderer/frame_buffer.h"
#include <entt/entt.hpp>

#include "ui/windows/viewport.h"

void Sandbox::DrawPropertiesWindow()
{
    ImGui::Begin("Properties");

    if(focused_object.type == FocusType::Entity) {
        Core::Level* _level = Core::LevelManager::GetCurrentLevel();

        Core::Entity_info& info = _level->GetComponent<Core::Entity_info>(focused_object.id);
        Core::Transform& transform = _level->GetComponent<Core::Transform>(focused_object.id);

        ImGui::Text("%s", info.name.c_str());
        ImGui::Text("Entity ID: %i", focused_object.id);

        {
            ImGui::NewLine();
            ImGui::Text("Transform");

            float pos[3] = {transform.position.x, transform.position.y, transform.position.z};

            bool value_changed = false;
            value_changed |= ImGui::InputFloat3("Position", pos);

            if(value_changed)
            {
                transform.position.x = pos[0];
                transform.position.y = pos[1];
                transform.position.z = pos[2];
                transform.dirty = true;
            }

            glm::quat rotation = transform.rotation;
            glm::vec3 euler_rotation = glm::eulerAngles(rotation);
            float rot[3] = {glm::degrees(euler_rotation.x), 
                glm::degrees(euler_rotation.y), 
                glm::degrees(euler_rotation.z)};

            value_changed = false;
            value_changed |= ImGui::InputFloat3("Rotation", rot);

            if(value_changed)
            {
                euler_rotation.x = glm::radians(rot[0]);
                euler_rotation.y = glm::radians(rot[1]);
                euler_rotation.z = glm::radians(rot[2]);
                glm::quat new_rot(euler_rotation);
                transform.rotation = new_rot;
                transform.dirty = true;
            }

            float scale[3] = {transform.scale.x, transform.scale.y, transform.scale.z};

            value_changed = false;
            value_changed |= ImGui::InputFloat3("Scale", scale);

            if(value_changed)
            {
                transform.scale.x = scale[0];
                transform.scale.y = scale[1];
                transform.scale.z = scale[2];
                transform.dirty = true;
            }
        }
    }
    else if (focused_object.type == FocusType::Asset) {
        
    }
    else {
    
    }

    ImGui::End();
}