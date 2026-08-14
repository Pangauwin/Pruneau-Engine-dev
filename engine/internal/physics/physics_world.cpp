#include "physics/physics_world.h"

#include <Jolt/Jolt.h>
#include <memory>
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/Memory.h"
#include "Jolt/RegisterTypes.h"

#define MAX_BODIES 10240
#define MAX_BODY_PAIRS 10240
#define MAX_CONTACT_CONSTRAINTS 10240

static Physics::PhysicsWorld* current_physics_world = nullptr;

void Physics::PhysicsWorld::Init()
{
    JPH::RegisterDefaultAllocator();

    m_temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

    m_job_system = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs,
        JPH::cMaxPhysicsBarriers,
        static_cast<int>(std::thread::hardware_concurrency()) - 1);

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

    current_physics_world = this;
}

void Physics::PhysicsWorld::Update(float dt)
{
    const int collision_step = 1;

    m_physics_system.Update(dt, collision_step, m_temp_allocator.get(), m_job_system.get());
}

void Physics::PhysicsWorld::Shutdown()
{
    JPH::UnregisterTypes();

    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

Physics::PhysicsWorld* Physics::PhysicsWorld::Get()
{
    return current_physics_world;
}