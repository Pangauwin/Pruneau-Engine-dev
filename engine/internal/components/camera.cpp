#include "components/camera.h"

#include "core/application.h"
#include "renderer/renderer.h"

#include "level/level_manager.h"
#include <entt/entt.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include "core/log/log.h"


void Core::CameraSystem::OnUpdate()
{
    entt::registry& _registry = LevelManager::GetCurrentLevel()->GetRegistry();

    for(auto& entity: _registry.view<Core::Camera>())
    {
        Core::Camera& _cam = _registry.get<Core::Camera>(entity);
        if(_cam.dirty)
        {
            if(_cam.config.type == CAMERA_TYPE_PERSPECTIVE)
            {
                CameraPerspectiveData* data = static_cast<CameraPerspectiveData*>(_cam.config.data);
                if(!data)
                {
                    Core::LogMessageError("Camera Error: could not find camera data!");
                    return;
                }
                _cam.projection = glm::perspective(glm::radians(data->fov), data->aspect, data->near_plane, data->far_plane);
            }

            else {
                CameraOrthographicData* data = static_cast<CameraOrthographicData*>(_cam.config.data);
                if(!data)
                {
                    Core::LogMessageError("Camera Error: could not find camera data!");
                    return;
                }
                _cam.projection = glm::ortho(data->left, data->right, data->bottom, data->top);
            }

            _cam.dirty = false;
        }
    }
}

unsigned int Core::CameraSystem::GetNextCameraIndex()
{
    static unsigned int camera_index = 0;
    camera_index++;
    return camera_index;
}