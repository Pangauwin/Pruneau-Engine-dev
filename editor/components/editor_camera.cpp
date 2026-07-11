#include "editor_camera.h"

#include "../core/engine_layer.h"

#include "core/application.h"
#include "core/component.h"
#include "components/transform.h"
#include "core/entity.h"
#include "input/input.h"
#include "glm/common.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include <imgui.h>

#include <GLFW/glfw3.h>

static double mouse_pos[2];

static Core::MOUSE_BUTTON_STATE previous_mouse_state = Core::MOUSE_BUTTON_STATE_RELEASED;
static bool previous_renderer_focused = false;

namespace Editor
{

EditorCamera::EditorCamera(Core::Entity* owner) : Core::Component(owner) 
{

}

EditorCamera::~EditorCamera()
{
    
}

void EditorCamera::OnUpdate(float dt)
{
    GLFWwindow* window = Core::Application::Get()->m_window->m_glfw_window;

    if(!EngineLayer::EngineLayer::Get()->is_renderer_focused)
    {
        if(previous_mouse_state == Core::MOUSE_BUTTON_STATE_PRESSED && previous_renderer_focused == true)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        return;
    }
    
    if(Core::Input::GetMouseButtonState(1) == Core::MOUSE_BUTTON_STATE_PRESSED && previous_mouse_state == Core::MOUSE_BUTTON_STATE_RELEASED)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, mouse_pos, &mouse_pos[1]);
    }

    if(Core::Input::GetMouseButtonState(1) == Core::MOUSE_BUTTON_STATE_PRESSED &&
     (Core::Input::GetMouseDelta()[0] != 0 || Core::Input::GetMouseDelta()[1] != 0))
    {
        const double delta_x = Core::Input::GetMouseDelta()[0];
        const double delta_y = Core::Input::GetMouseDelta()[1];

        m_yaw -= delta_x * camera_sensitivity;
        m_pitch -= delta_y * camera_sensitivity;

        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

        glm::quat q_yaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 1, 0));
        glm::quat q_pitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));

        glm::quat new_rotation = q_yaw * q_pitch;

        GetOwner()->GetComponent<Core::Transform>()->SetRotation(new_rotation);
    }

    bool has_moved = false;
    glm::vec3 movement_direction = glm::vec3(0, 0, 0);

    if(Core::Input::GetKeyState('W') == Core::KEY_STATE_PRESSED)
    {
        has_moved = true;
        movement_direction += GetOwner()->GetComponent<Core::Transform>()->GetForward();
    }

    if(Core::Input::GetKeyState('S') == Core::KEY_STATE_PRESSED)
    {
        has_moved = true;
        movement_direction -= GetOwner()->GetComponent<Core::Transform>()->GetForward();
    }

    if(Core::Input::GetKeyState('D') == Core::KEY_STATE_PRESSED)
    {
        has_moved = true;
        movement_direction += GetOwner()->GetComponent<Core::Transform>()->GetRight();
    }

    if(Core::Input::GetKeyState('A') == Core::KEY_STATE_PRESSED)
    {
        has_moved = true;
        movement_direction -= GetOwner()->GetComponent<Core::Transform>()->GetRight();
    }

    if(Core::Input::GetKeyState('E') == Core::KEY_STATE_PRESSED)
    {
        has_moved = true;
        movement_direction += GetOwner()->GetComponent<Core::Transform>()->GetUp();
    }

    if(Core::Input::GetKeyState('Q') == Core::KEY_STATE_PRESSED)
    {
        has_moved = true;
        movement_direction -= GetOwner()->GetComponent<Core::Transform>()->GetUp();
    }

    if(has_moved)
    {    
        if(glm::length(movement_direction) > 0)
            movement_direction = glm::normalize(movement_direction);
        movement_direction *= camera_speed * dt;

        GetOwner()->GetComponent<Core::Transform>()->Translate(movement_direction);
    }

    previous_mouse_state = Core::Input::GetMouseButtonState(1);
    previous_renderer_focused = EngineLayer::EngineLayer::Get()->is_renderer_focused;

    //glfwSetCursorPos(window, mouse_pos[0], mouse_pos[1]);

    double xpos2, ypos2;
    //glfwGetCursorPos(window, &xpos2, &ypos2);

    /*Core::LogMessageInfo("Mouse pos: " + std::to_string(xpos2) 
        + "," + std::to_string(ypos2));

    Core::LogMessageInfo("Mouse desired pos: " + std::to_string(mouse_pos[0]) 
        + "," + std::to_string(mouse_pos[1]));

    Core::LogMessageInfo("Mouse delta: " + std::to_string(Core::Input::GetMouseDelta()[0]) 
        + "," + std::to_string(Core::Input::GetMouseDelta()[1]));*/

    //Core::Input::previous_mouse_position[0] = mouse_pos[0];
    //Core::Input::previous_mouse_position[1] = mouse_pos[1];

    //TODO : Clear the previous mouse pos (idk how) to make the camera can rotate and its rotation movement ends !
}
}