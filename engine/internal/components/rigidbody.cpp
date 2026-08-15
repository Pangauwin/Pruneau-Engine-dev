#include "components/rigidbody.h"
#include "Jolt/Math/Quat.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/EActivation.h"
#include "components/transform.h"
#include "core/application.h"
#include "entt/entity/fwd.hpp"
#include "entt/signal/fwd.hpp"
#include "level/level_events.h"
#include "level/level_manager.h"
#include "physics/physics_events.h"
#include "physics/physics_layers.h"
#include "physics/physics_world.h"
#include <memory>

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

        _rb.dirty = false; // TODO: instead of rebuilding the physics body for every change, better architecture would be:
        /*
        * - Have two dirty flags, one related to body properties, the other one to the simulation
        * - Rebuild the physics body on simulation launch
        */
    }
}

void Core::RigidBodySystem::OnLateUpdate()
{
    SyncTransform();
}

void Core::RigidBodySystem::ConnectPhysicsEvents()
{
    entt::dispatcher& _dispatcher = Core::_dispatcher;

    _dispatcher.sink<Physics::OnSimulationBegin>().connect<&Core::RigidBodySystem::OnSimulationBegin>(*this);
    _dispatcher.sink<Physics::OnSimulationEnd>().connect<&Core::RigidBodySystem::OnSimulationEnd>(*this);
}

void Core::RigidBodySystem::OnSimulationBegin(const Physics::OnSimulationBegin& _event)
{
    entt::registry& _reg = Core::LevelManager::GetCurrentLevel()->GetRegistry();

    Core::Rigidbody& _rb = _reg.get<Core::Rigidbody>(_event._ent);
    Core::Transform& _tr = _reg.get<Core::Transform>(_event._ent);

    _rb.starting_position = _tr.position;
    _rb.starting_rotation = _tr.rotation;

    JPH::RVec3 position(_tr.position.x, _tr.position.y, _tr.position.z);
    JPH::Quat rotation(_tr.rotation.x, _tr.rotation.y, _tr.rotation.z, _tr.rotation.w);

    _rb.body_settings = std::make_unique<JPH::BodyCreationSettings>(_rb.shape_settings.Create().Get(), 
        position, 
        rotation, 
        _rb.motion_type,
        Physics::Layers::MOVING
    );

    if(!_rb.simulating)
    {
        if(_rb._body.IsInvalid())
        {
            _rb._body = Physics::PhysicsWorld::Get()->GetBodyInterface().CreateAndAddBody(*_rb.body_settings.get(), JPH::EActivation::Activate);
        }
        else {
            Physics::PhysicsWorld::Get()->GetBodyInterface().SetPositionAndRotation(_rb._body, position, rotation, JPH::EActivation::Activate);
            //Physics::PhysicsWorld::Get()->GetBodyInterface().AddBody(_rb._body, JPH::EActivation::Activate);
        }
    }

    else {
        Core::LogMessageInfo("Restarting Physics Simulation...");

        JPH::RVec3 starting_position(_rb.starting_position.x, _rb.starting_position.y, _rb.starting_position.z);
        JPH::Quat starting_rotation(_rb.starting_rotation.x, _rb.starting_rotation.y, _rb.starting_rotation.z, _rb.starting_rotation.w);;

        _rb.body_settings->mPosition = starting_position;
        _rb.body_settings->mRotation = starting_rotation;

        if(_rb._body.IsInvalid())
        {
            _rb._body = Physics::PhysicsWorld::Get()->GetBodyInterface().CreateAndAddBody(*_rb.body_settings.get(), JPH::EActivation::Activate);
        }

        Physics::PhysicsWorld::Get()->GetBodyInterface().SetPositionAndRotation(_rb._body, starting_position, starting_rotation, JPH::EActivation::Activate);
    }

    _rb.simulating = true;
}

void Core::RigidBodySystem::OnSimulationEnd(const Physics::OnSimulationEnd& _event)
{
    entt::registry& _reg = Core::LevelManager::GetCurrentLevel()->GetRegistry();
    Core::Rigidbody& _rb = _reg.get<Core::Rigidbody>(_event._ent);

    if(!_rb.simulating)
    {
        Core::LogMessageError("RigidBody not simulating ! Can't stop simulation !");
        return;
    }

    if(!_rb._body.IsInvalid())
    {
        JPH::BodyInterface& _body_interface = Physics::PhysicsWorld::Get()->GetBodyInterface();
        _body_interface.RemoveBody(_rb._body);
        _body_interface.DestroyBody(_rb._body); //TODO: Fix this bug (why can't I destroy this body => error)
    }

    Core::Transform& _tr = _reg.get<Core::Transform>(_event._ent);
    _tr.position = _rb.starting_position;
    _tr.rotation = _rb.starting_rotation;

    _rb.simulating = false;
}