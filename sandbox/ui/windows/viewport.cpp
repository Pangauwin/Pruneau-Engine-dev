#include "viewport.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <ImGuizmo.h>
#include "components/transform.h"
#include "components/camera.h"

#include "core/application.h"
#include "level/level.h"
#include "level/level_manager.h"

#include "layer/sandbox_layer.h"

#include "glm/gtc/type_ptr.hpp"
#include "renderer/renderer.h"

static ImVec2 m_viewport_size = {0.0f, 0.0f};

ImVec2 Sandbox::viewport_position = {0.0f, 0.0f};

static ImGuizmo::MODE s_guizmo_mode = ImGuizmo::MODE::LOCAL;
static ImGuizmo::OPERATION s_guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
static bool s_snap = false;
static float s_transform_snap[3] = {1.f, 1.f, 1.f};
static float s_scale_snap[3] = {1.f, 1.f, 1.f};
static float s_rotate_snap[3] = {45.f, 45.f, 45.f};

void Sandbox::DrawViewport() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
    {
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::Begin("Viewport", nullptr, window_flags);

    ImVec2 current_viewport_size = ImGui::GetContentRegionAvail();

    Renderer::Renderer* _renderer = Core::Application::Get()->m_renderer.get();

    uint32_t texture_id = _renderer->m_frame_buffer.GetColorAttachmentRendererID();

    Sandbox::viewport_position = ImGui::GetCursorScreenPos();

    ImGui::Image(
        (ImTextureID)(uint32_t)texture_id,
        ImVec2((float)_renderer->m_frame_buffer.width, (float)_renderer->m_frame_buffer.height),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    {
        Core::Level* _level = Core::LevelManager::GetCurrentLevel();

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        for(auto& entity: _level->GetRegistry().view<Core::Camera, Core::Transform>())
        {
            if(_level->GetComponent<Core::Camera>(entity).index == _level->camera_index)
            {
                Core::Transform& _transf = _level->GetComponent<Core::Transform>(entity);
                view = glm::inverse(_transf.world_transform);
                projection = _level->GetComponent<Core::Camera>(entity).projection;
            }
        }


        if(_level->GetRegistry().valid(static_cast<entt::entity>(focused_object.id)))
        {
                
            Core::Transform transform = _level->GetComponent<Core::Transform>(focused_object.id);

            glm::mat4 parent_world = glm::mat4(1.0f);
            if (_level->GetRegistry().all_of<Core::Parent>(static_cast<entt::entity>(focused_object.id)))
            {
                Core::Entity parent = _level->GetComponent<Core::Parent>(focused_object.id).entity;
                if(parent)
                    parent_world = _level->GetComponent<Core::Transform>(parent).world_transform;
            }

            glm::mat4 local_matrix = glm::translate(glm::mat4(1.0f), transform.position) ;
            local_matrix *= glm::mat4_cast(transform.rotation);
            local_matrix = glm::scale(local_matrix, transform.scale);

            glm::mat4 handle_world_matrix = transform.world_transform;

            ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

            Renderer::Renderer* _renderer = Core::Application::Get()->m_renderer.get();

            ImGuizmo::SetRect(
                Sandbox::viewport_position.x,
                Sandbox::viewport_position.y,
                _renderer->m_frame_buffer.width,
                _renderer->m_frame_buffer.height
            );

            ImGuizmo::Manipulate(
                glm::value_ptr(view), 
                glm::value_ptr(projection), 
                s_guizmo_operation,
                s_guizmo_mode, 
                glm::value_ptr(handle_world_matrix),
                NULL,
                s_snap ? 
                    s_guizmo_operation == ImGuizmo::TRANSLATE ?  &s_transform_snap[0] :
                    s_guizmo_operation == ImGuizmo::ROTATE ? &s_rotate_snap[0] :
                    s_guizmo_operation == ImGuizmo::SCALE ? &s_scale_snap[0]
                : NULL : NULL
            );

            if (ImGuizmo::IsUsing())
            {
                glm::mat4 new_local_matrix = glm::inverse(parent_world) * handle_world_matrix;

                glm::vec3 pos, rot_deg, scale;
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(new_local_matrix), 
                    glm::value_ptr(pos), 
                    glm::value_ptr(rot_deg), 
                    glm::value_ptr(scale)
                );

                Core::Transform& _transf = _level->GetComponent<Core::Transform>(focused_object.id);

                _transf.position = pos;
                _transf.rotation = glm::quat(glm::radians(rot_deg));
                _transf.scale = scale;
                _transf.dirty = true;
            }
        }
    }


    ImGui::End();

    ImGui::Begin("Transform tools", nullptr, ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Mode");
    ImGui::SameLine();
    if(ImGui::Button("L"))
    {
        s_guizmo_mode = ImGuizmo::LOCAL;
    }
    ImGui::SameLine();
    if(ImGui::Button("W"))
    {
        s_guizmo_mode = ImGuizmo::WORLD;
    }
    ImGui::SameLine();


    ImGui::Text("Operation");
    ImGui::SameLine();
    if(ImGui::Button("T"))
    {
        s_guizmo_operation = ImGuizmo::TRANSLATE;
    }
    ImGui::SameLine();
    if(ImGui::Button("R"))
    {
        s_guizmo_operation = ImGuizmo::ROTATE;
    }
    ImGui::SameLine();
    if(ImGui::Button("S"))
    {
        s_guizmo_operation = ImGuizmo::SCALE;
    }

    ImGui::SameLine();
    ImGui::Checkbox("Snap", &s_snap);

    ImGui::End();


    if(current_viewport_size.x != m_viewport_size.x || 
        current_viewport_size.y != m_viewport_size.y)
        _renderer->m_frame_buffer.Resize((uint32_t)current_viewport_size.x, (uint32_t)current_viewport_size.y);

    m_viewport_size = current_viewport_size;
}