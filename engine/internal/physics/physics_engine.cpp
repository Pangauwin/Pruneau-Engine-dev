#include "physics/physics_engine.h"

#include <Jolt/Jolt.h>
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/Memory.h"
#include "Jolt/RegisterTypes.h"
#include "physics/physics_layers.h"

#define MAX_BODIES 10240
#define MAX_BODY_PAIRS 10240
#define MAX_CONTACT_CONSTRAINTS 10240

void Physics::PhysicsEngine::Init()
{
    JPH::RegisterDefaultAllocator();

    JPH::Factory::sInstance = new JPH::Factory();

    JPH::RegisterTypes();

    /*m_broad_phase_layer = new Physics::Layers::BroadPhaseLayerInterfaceImpl();

    m_physics_system.Init(
        MAX_BODIES, 
        0, 
        MAX_BODY_PAIRS, 
        MAX_CONTACT_CONSTRAINTS, 
        object, const ObjectVsBroadPhaseLayerFilter &inObjectVsBroadPhaseLayerFilter, const ObjectLayerPairFilter &inObjectLayerPairFilter);
    */
    m_body_interface = &m_physics_system.GetBodyInterface();
}

void Physics::PhysicsEngine::Shutdown()
{
    JPH::UnregisterTypes();

    delete JPH::Factory::sInstance;
}