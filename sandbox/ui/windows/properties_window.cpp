#include "properties_window.h"

#include <functional>
#include <imgui.h>
#include "components/model_renderer.h"
#include "components/transform.h"
#include "components/camera.h"
#include "entt/entity/fwd.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include "layer/sandbox_layer.h"

#include "level/level.h"
#include "level/level_manager.h"
#include <entt/entt.hpp>

std::vector<std::function<void(entt::registry& _reg, entt::entity _ent)>> Sandbox::draw_functions;

void Sandbox::DrawPropertiesWindow()
{
    ImGui::Begin("Properties");

    if(focused_object.type == FocusType::Entity) {
        Core::Level* _level = Core::LevelManager::GetCurrentLevel();

        Core::Entity_info& info = _level->GetComponent<Core::Entity_info>(focused_object.id);
        Core::Transform& transform = _level->GetComponent<Core::Transform>(focused_object.id);

        ImGui::Text("%s", info.name.c_str());
        ImGui::Text("Entity ID: %i", focused_object.id);

        for(auto draw_function : Sandbox::draw_functions)
        {
            draw_function(_level->GetRegistry(), static_cast<entt::entity>(focused_object.id));
        }
    }
    else if (focused_object.type == FocusType::Asset) {
        // TODO: Implement
    }
    else {
        
    }

    ImGui::End();
}

void Sandbox::RegisterEngineComponentsDrawFunctions()
{
    RegisterDrawFunctionComponent<Core::Transform>("Transform", 
        [](Core::Transform& _t) {

            float pos[3] = {_t.position.x, _t.position.y, _t.position.z};

            bool value_changed = false;
            value_changed |= ImGui::InputFloat3("Position", pos);

            if(value_changed)
            {
                _t.position.x = pos[0];
                _t.position.y = pos[1];
                _t.position.z = pos[2];
                _t.dirty = true;
            }

            glm::quat rotation = _t.rotation;
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
                _t.rotation = new_rot;
                _t.dirty = true;
            }

            float scale[3] = {_t.scale.x, _t.scale.y, _t.scale.z};

            value_changed = false;
            value_changed |= ImGui::InputFloat3("Scale", scale);

            if(value_changed)
            {
                _t.scale.x = scale[0];
                _t.scale.y = scale[1];
                _t.scale.z = scale[2];
                _t.dirty = true;
            }
        });

    RegisterDrawFunctionComponent<Core::Camera>("Camera", 
        [](Core::Camera& _t) {
            ImGui::DragInt("Camera index", (int*)&_t.index);

            static const char* camera_modes[] = {
                "perspective",
                "orthographic"
            };

            const char* current_camera_mode = 
                camera_modes[_t.config.type == Core::CAMERA_TYPE_PERSPECTIVE ? 0 : 1];
            
            const char* previous_camera_mode = current_camera_mode;

            if(ImGui::BeginCombo("Camera type", current_camera_mode))
            {
                for(int n = 0; n < IM_ARRAYSIZE(camera_modes); ++n)
                {
                    bool is_selected = std::string(camera_modes[n]) == current_camera_mode;
                    if (ImGui::Selectable(camera_modes[n], is_selected))
                        current_camera_mode = camera_modes[n];
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if(current_camera_mode != previous_camera_mode)
            {
                delete _t.config.data;

                if(current_camera_mode == camera_modes[0])
                {
                    _t.config.data = new Core::CameraPerspectiveData();
                    _t.config.type = Core::CAMERA_TYPE_PERSPECTIVE;
                }
                else {
                    _t.config.data = new Core::CameraOrthographicData();
                    _t.config.type = Core::CAMERA_TYPE_ORTHOGRAPHIC;
                }

                _t.dirty = true;
            }

            if(current_camera_mode == camera_modes[0])
            {
                Core::CameraPerspectiveData* _data = static_cast<Core::CameraPerspectiveData*>(_t.config.data);
                
                if(ImGui::InputFloat("FOV", &_data->fov)) _t.dirty = true;
                if(ImGui::InputFloat("near", &_data->near_plane)) _t.dirty = true;
                if(ImGui::InputFloat("far", &_data->far_plane)) _t.dirty = true;

                ImGui::Text("Aspect: %.3f", _data->aspect);
            }
            else 
            {
                Core::CameraOrthographicData* _data = static_cast<Core::CameraOrthographicData*>(_t.config.data);
            
                if(ImGui::InputFloat("top", &_data->top)) _t.dirty = true;
                if(ImGui::InputFloat("bottom", &_data->bottom)) _t.dirty = true;
                if(ImGui::InputFloat("right", &_data->right)) _t.dirty = true;
                if(ImGui::InputFloat("left", &_data->left)) _t.dirty = true;
            }
        });
    
    RegisterDrawFunctionComponent<Core::ModelRenderer>("Model Renderer", 
        [](Core::ModelRenderer& _t)
    {
        ImGui::InputInt("Model ID", (int*)&_t.model_id);
    });
    
}