#pragma once

#include <Jolt/Jolt.h>
#include <thread>
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "physics/physics_layers.h"
namespace Physics
{

class PhysicsWorld {

public:
    void Init();
    void Update(float dt);
    void Shutdown();

    JPH::BodyInterface& GetBodyInterface() { return *m_body_interface; };

private:
    JPH::PhysicsSystem m_physics_system;
    JPH::BodyInterface* m_body_interface = nullptr;

    Physics::Layers::BroadPhaseLayerInterfaceImpl m_broad_phase_layer;
    Physics::Layers::ObjectLayerPairFilterImpl m_object_layer_pair_filter;
    Physics::Layers::ObjectVsClassBroadPhaseLayerImpl m_object_vs_class_broad_phase_layer;

    JPH::TempAllocatorImpl m_temp_allocator{10 * 1024 * 1024};
    JPH::JobSystemThreadPool m_job_system{
        JPH::cMaxPhysicsJobs,
        JPH::cMaxPhysicsBarriers,
        static_cast<int>(std::thread::hardware_concurrency()) - 1
    };
};

}