#include "physics/physics_world.h"

#include <Jolt/Jolt.h>
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/Memory.h"
#include "Jolt/RegisterTypes.h"

#define MAX_BODIES 10240
#define MAX_BODY_PAIRS 10240
#define MAX_CONTACT_CONSTRAINTS 10240

void Physics::PhysicsWorld::Init()
{
    JPH::RegisterDefaultAllocator();

    JPH::Factory::sInstance = new JPH::Factory();

    JPH::RegisterTypes();

    m_physics_system.Init(
        MAX_BODIES, 
        0, 
        MAX_BODY_PAIRS, 
        MAX_CONTACT_CONSTRAINTS, 
        m_broad_phase_layer,
        m_object_vs_class_broad_phase_layer, 
        m_object_layer_pair_filter);
    
    m_body_interface = &m_physics_system.GetBodyInterface();
}

void Physics::PhysicsWorld::Update(float dt)
{
    const int collision_step = 1;

    m_physics_system.Update(dt, collision_step, &m_temp_allocator, &m_job_system);
}

void Physics::PhysicsWorld::Shutdown()
{
    JPH::UnregisterTypes();

    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}