#pragma once

#include <Jolt/Jolt.h>
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "physics/physics_layers.h"
namespace Physics
{

class PhysicsEngine {

public:
    void Init();

    void Update(float dt);

    void Shutdown();

private:
    JPH::PhysicsSystem m_physics_system;
    JPH::BodyInterface* m_body_interface;

    Physics::Layers::BroadPhaseLayerInterfaceImpl* m_broad_phase_layer;
    Physics::Layers::ObjectLayerPairFilterImpl m_object_layer_pair_filter;
};

}