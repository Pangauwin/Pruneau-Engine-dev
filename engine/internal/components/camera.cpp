#include "components/camera.h"

#include "core/application.h"
#include "renderer/renderer.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Core {

    static bool registered = []()
        {
            AutoRegisterComponent<Camera>::Register("Camera");
            return true;
        }();

}

Core::Camera::Camera(Entity* owner) : Component(owner), config(CAMERA_TYPE_PERSPECTIVE, new CameraPerspectiveData()), m_projection(1.0f), updated(true) // Used by editor
{
    CameraPerspectiveData* data = static_cast<CameraPerspectiveData*>(config.data);

    m_projection = glm::perspective(glm::radians(data->fov), data->aspect, data->near_plane, data->far_plane);

    // Defensive: only register if renderer exists
    if (Renderer::Renderer::Get())
        Renderer::Renderer::Get()->RegisterCamera(this);
    else
        Core::LogMessageError("Renderer not detected ! Camera registration impossible !");
}

Core::Camera::Camera(Entity* _owner, CameraConfig& _camera_config) : Component(_owner), config(_camera_config), m_projection(glm::mat4(1.0f)), updated(true)
{
    if (config.type == CAMERA_TYPE_ORTHOGRAPHIC)
    {
        CameraOrthographicData* data = static_cast<CameraOrthographicData*>(config.data);

        m_projection = glm::ortho(data->left, data->right, data->bottom, data->top);
    }

    else
    {
        CameraPerspectiveData* data = static_cast<CameraPerspectiveData*>(config.data);

        m_projection = glm::perspective(glm::radians(data->fov), data->aspect, data->near_plane, data->far_plane);
    }

    if (Renderer::Renderer::Get())
        Renderer::Renderer::Get()->RegisterCamera(this);
    else
        Core::LogMessageError("Renderer not detected ! Camera registration impossible !");
}

Core::Camera::~Camera()
{
    if (Renderer::Renderer::Get())
        Renderer::Renderer::Get()->UnRegisterCamera(this);
    else
        Core::LogMessageError("Renderer not detected, Camera registration impossible !");
}

void Core::Camera::SetCameraConfig(CameraConfig& _camera_config)
{
    // Replace our stored config with a deep copy and mark projection dirty
    config = _camera_config; // CameraConfig has safe copy semantics now
    updated = false;
}

void Core::Camera::SetAspect(float _aspect)
{
    if (!(config.type == CAMERA_TYPE_PERSPECTIVE)) return;

    CameraPerspectiveData* data = static_cast<CameraPerspectiveData*>(config.data);

    data->aspect = _aspect;

    updated = false;
}

glm::mat4* Core::Camera::GetPerspective()
{
    if (updated)
        return &m_projection;

    if (config.type == CAMERA_TYPE_ORTHOGRAPHIC)
    {
        CameraOrthographicData* data = static_cast<CameraOrthographicData*>(config.data);

        m_projection = glm::ortho(data->left, data->right, data->bottom, data->top);
    }

    else // if(m_config.type == CAMERA_TYPE_PERSPECTIVE)
    {
        CameraPerspectiveData* data = static_cast<CameraPerspectiveData*>(config.data);

        m_projection = glm::perspective(glm::radians(data->fov), data->aspect, data->near_plane, data->far_plane);
    }

    updated = true;

    return &m_projection;
}
