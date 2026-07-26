#include "scene_camera.h"

#include "components/transform.h"
#include "entt/entity/entity.hpp"
#include "input/input.h"
#include "level/level_manager.h"
#include "core/time.h"

#include <entt/entt.hpp>

#include "glm/common.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"

void Sandbox::SceneCameraSystem::OnUpdate()
{
    Core::Level* lvl = Core::LevelManager::GetCurrentLevel();

    for(auto& ent: lvl->GetRegistry().view<SceneCamera, Core::Transform>())
    {
        if(Core::Input::GetMouseButtonState(1) == Core::MOUSE_BUTTON_STATE_PRESSED)
        {
            const double delta_x = Core::Input::GetMouseDelta()[0];
            const double delta_y = Core::Input::GetMouseDelta()[1];

            SceneCamera& sc_cam = lvl->GetComponent<SceneCamera>(entt::to_integral(ent));

            sc_cam.yaw -= delta_x * sc_cam.camera_sensitivity;
            sc_cam.pitch -= delta_y * sc_cam.camera_sensitivity;

            sc_cam.pitch = glm::clamp(sc_cam.pitch, -89.0f, 89.0f);
            glm::quat q_yaw = glm::angleAxis(glm::radians(sc_cam.yaw), glm::vec3(0, 1, 0));
            glm::quat q_pitch = glm::angleAxis(glm::radians(sc_cam.pitch), glm::vec3(1, 0, 0));

            glm::quat new_rotation = q_yaw * q_pitch;

            Core::Transform& transf = lvl->GetComponent<Core::Transform>(entt::to_integral(ent));
            transf.dirty = true;
            transf.rotation = new_rotation;

            bool has_moved = false;
            glm::vec3 movement_direction = glm::vec3(0, 0, 0);

            if(Core::Input::GetKeyState('W') == Core::KEY_STATE_PRESSED)
            {
                has_moved = true;
                movement_direction += transf.forward;
            }

            if(Core::Input::GetKeyState('S') == Core::KEY_STATE_PRESSED)
            {
                has_moved = true;
                movement_direction -= transf.forward;
            }

            if(Core::Input::GetKeyState('D') == Core::KEY_STATE_PRESSED)
            {
                has_moved = true;
                movement_direction += transf.right;
            }

            if(Core::Input::GetKeyState('A') == Core::KEY_STATE_PRESSED)
            {
                has_moved = true;
                movement_direction -= transf.right;
            }

            if(Core::Input::GetKeyState('E') == Core::KEY_STATE_PRESSED)
            {
                has_moved = true;
                movement_direction += transf.up;
            }

            if(Core::Input::GetKeyState('Q') == Core::KEY_STATE_PRESSED)
            {
                has_moved = true;
                movement_direction -= transf.up;
            }

            if(has_moved)
            {    
                if(glm::length(movement_direction) > 0)
                    movement_direction = glm::normalize(movement_direction);
                movement_direction *= sc_cam.camera_speed * Time::delta_time;

                transf.position += movement_direction;
            }
        }
    }
}