#include "components/rigidbody.h"
#include "Jolt/Math/Quat.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/EActivation.h"
#include "components/transform.h"
#include "entt/entity/fwd.hpp"
#include "level/level_manager.h"
#include "physics/physics_world.h"

void Core::RigidBodySystem::SyncTransform()
{
    entt::registry& _reg = Core::LevelManager::GetCurrentLevel()->GetRegistry();
    JPH::BodyInterface& body_interface = Physics::PhysicsWorld::Get()->GetBodyInterface();

    for(auto& _ent : _reg.view<Core::Rigidbody, Core::Transform>())
    {
        Core::Rigidbody& rb = _reg.get<Core::Rigidbody>(_ent);

        if(rb.motion_type == JPH::EMotionType::Static)
            continue;

        if(!body_interface.IsActive(rb._body))
            continue;

        JPH::RVec3 position;
        JPH::Quat rotation;

        body_interface.GetPositionAndRotation(rb._body, position, rotation);

        Core::Transform& transform = _reg.get<Core::Transform>(_ent);

        transform.position = {position.GetX(), position.GetY(), position.GetZ()};
        transform.rotation = {rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ()};
        transform.dirty = true;
    }
}

void Core::RigidBodySystem::OnUpdate()
{
    entt::registry& _reg = Core::LevelManager::GetCurrentLevel()->GetRegistry();

    for(auto& _ent : _reg.view<Core::Rigidbody, Core::Transform>())
    {
        Core::Rigidbody& _rb = _reg.get<Core::Rigidbody>(_ent);

        if(!_rb.dirty) continue;

        Core::Transform& _tr = _reg.get<Core::Transform>(_ent);
        JPH::RVec3 position(_tr.position.x, _tr.position.y, _tr.position.z);
        JPH::Quat rotation(_tr.rotation.x, _tr.rotation.y, _tr.rotation.z, _tr.rotation.w);;

        _rb.body_settings = std::make_unique<JPH::BodyCreationSettings>(&_rb.shape_settings, 
            position, 
            rotation, 
            _rb.motion_type,
            _rb._layer
        );

        if(_rb._body.IsInvalid())
        {
            _rb._body = Physics::PhysicsWorld::Get()->GetBodyInterface().CreateAndAddBody(*_rb.body_settings.get(), JPH::EActivation::Activate);
        }

        else {
            Physics::PhysicsWorld::Get()->GetBodyInterface().SetPositionAndRotation(_rb._body, position, rotation, JPH::EActivation::Activate);
        }

        _rb.dirty = false;
    }
}

void Core::RigidBodySystem::OnLateUpdate()
{
    SyncTransform();
}