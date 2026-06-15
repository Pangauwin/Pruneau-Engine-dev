#include "component_displayer.h"
#include "asset/asset_manager.h"
#include "components/camera.h"
#include "components/model_renderer.h"
#include "components/transform.h"
#include "core/component.h"
#include "core/entity.h"

#include "core/log/log.h"

#include "utils/class.h"

#include <cxxabi.h>

#include <imgui.h>

#include <ImGuizmo.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

using InspectorFunction = std::function<void(Core::Component&)>;
static std::unordered_map<std::type_index, InspectorFunction> s_inspectors;

static bool registered = false;

void RegisterComponentDrawFunctions();


void Editor::ComponentDisplayer::DrawEntityInterface(Core::Entity& _entity)
{
    if(!registered)
    {
        RegisterComponentDrawFunctions();
        registered = true;
    }

    ImGui::InputText("Name##entity_name" , &_entity.name);

    for (auto& i : _entity.components)
    {
        std::string comp_name = Utils::GetTypeName(*i.second.get());

        std::string result = comp_name.substr(comp_name.find_last_of(":") + 1);

        ImGui::BeginGroup();

        bool enabled = true; // TODO : Implement Component enable
        ImGui::Checkbox("##enabled", &enabled);

        ImGui::SameLine();

        if (ImGui::CollapsingHeader(result.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto it = s_inspectors.find(typeid(*i.second));

            if(it != s_inspectors.end())
            {
                it->second(*i.second);
            }
        }

        ImGui::EndGroup();
    }

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponentPopup");

    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        for (const auto& [type, info] : Core::ComponentRegistry::All())
        {
            if (ImGui::MenuItem(info.name.c_str()) && !(_entity.components[type]))
            {
                _entity.components[type] = info.factory(&_entity);
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void RegisterComponentDrawFunctions()
{
    s_inspectors[typeid(Core::Transform)] = [](Core::Component& c)
    {
        auto& _transform_component = static_cast<Core::Transform&>(c);

        bool update_matrix = false;

        if (ImGui::DragFloat3("Position", &_transform_component.m_position.x, 0.1f))
        {
            update_matrix = true;
        }
        if (ImGui::DragFloat3("Scale", &_transform_component.m_scale.x, 0.1f))
        {
            update_matrix = true;
        }

        glm::vec3 euler = glm::degrees(glm::eulerAngles(_transform_component.m_rotation));

        if(ImGui::DragFloat3("Rotation", &euler.x, 0.1f))
        {
            glm::vec3 radians = glm::radians(euler);
            _transform_component.m_rotation = glm::quat(radians);

            update_matrix = true;
        }

        if (update_matrix)
        {
            _transform_component.UpdateMatrix();
        }
    };

    s_inspectors[typeid(Core::ModelRenderer)] = [](Core::Component& c) {
        auto& _model_renderer_component = static_cast<Core::ModelRenderer&>(c);

        int _id = _model_renderer_component.m_model_id;
        if(ImGui::InputInt("Model ID", &_id))
        {
            if(auto model_asset = Core::AssetManager::GetAsset<Core::ModelAsset>(_id))
            {
                _model_renderer_component.SetModelID(_id);
            }
        }
    };

    s_inspectors[typeid(Core::Camera)] = [](Core::Component& c) {
        auto& _camera_component = static_cast<Core::Camera&>(c);

        if (_camera_component.config.type == Core::CAMERA_TYPE_PERSPECTIVE)
        {
            Core::CameraPerspectiveData* data = static_cast<Core::CameraPerspectiveData*>(_camera_component.config.data);

            if(ImGui::DragFloat("Field of view", &data->fov))
                _camera_component.updated = false;
            if(ImGui::DragFloat("Far Plane", &data->far_plane))
                _camera_component.updated = false;
            if(ImGui::DragFloat("Near Plane", &data->near_plane))
                _camera_component.updated = false;
        }
    };
}