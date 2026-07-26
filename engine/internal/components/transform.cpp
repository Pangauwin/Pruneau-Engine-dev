#include "components/transform.h"

#include "entt/entity/fwd.hpp"
#include "level/level.h"
#include "level/level_manager.h"

#include "core/log/log.h"

void Core::TransformSystem::OnUpdate()
{
    entt::registry& _registry = LevelManager::GetCurrentLevel()->GetRegistry();
    auto view = _registry.view<Transform>();

    for(auto& entity: view)
    {
        Transform& data = _registry.get<Transform>(entity);

        if(!data.dirty) continue;

        UpdateMatrix(data);
    }
}

void Core::TransformSystem::UpdateMatrix(Core::Transform& _transform)
{
    entt::registry& _registry = Core::LevelManager::GetCurrentLevel()->GetRegistry();

    if(_transform.parent)
    {
        if(!_registry.valid(static_cast<entt::entity>(_transform.parent)))
        {
            Core::LogMessageError("Parent Entity not valid! Can't update transform !");
            return;
        }

        if(_registry.all_of<Core::Transform>(static_cast<entt::entity>(_transform.parent)))
        {
            Core::Transform& _parent_transform = _registry.get<Core::Transform>(static_cast<entt::entity>(_transform.parent));

            if(_parent_transform.dirty)
            {
                UpdateMatrix(_parent_transform);
            }
        }
    }

    _transform.local_transform = glm::mat4(1.0f);
    _transform.local_transform = glm::translate(_transform.local_transform, _transform.position);
    _transform.local_transform *= glm::mat4_cast(_transform.rotation);
    _transform.local_transform = glm::scale(_transform.local_transform, _transform.scale);

    glm::mat3 m_mat3_local_transform = glm::mat3(_transform.local_transform);

    _transform.up = m_mat3_local_transform * glm::vec3(0.0f, 1.0f, 0.0f);
    _transform.forward = m_mat3_local_transform * glm::vec3(0.0f, 0.0f, -1.0f);
    _transform.right = m_mat3_local_transform * glm::vec3(1.0f, 0.0f, 0.0f);

    if(_transform.parent)
    {
        if(_registry.all_of<Core::Transform>(static_cast<entt::entity>(_transform.parent)))
        {
            Core::Transform _parent_transform = _registry.get<Core::Transform>(static_cast<entt::entity>(_transform.parent));
            _transform.world_transform = _parent_transform.world_transform * _transform.local_transform;
        }
        else
        {
            _transform.world_transform = _transform.local_transform;
        }
    }
    else {
        _transform.world_transform = _transform.local_transform;
    }

    _transform.dirty = false;

    if(_transform.children.size() != 0)
    {
        for (auto child: _transform.children) {
            Core::Transform& _child_transform = _registry.get<Core::Transform>(static_cast<entt::entity>(child));
            _child_transform.dirty = true;
        }
    }
}