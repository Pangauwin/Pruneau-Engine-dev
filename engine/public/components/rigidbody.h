#pragma once

#include "core/component/component.h"
#include "core/component/component_system.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "physics/physics_events.h"
#include "rapidjson/document.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include <memory>

namespace Core {

struct Rigidbody : public Component
{
    // TODO: find a way to serialize this (register again)
    JPH::EMotionType motion_type = JPH::EMotionType::Dynamic;
    JPH::BoxShapeSettings shape_settings = JPH::BoxShapeSettings(JPH::RVec3(1.0f, 1.0f, 1.0f));
    JPH::ObjectLayer _layer = 1;
    float mass = 1.0f;

    JPH::BodyID _body; //runtime values
    JPH::CollideShapeResult shape;
    std::unique_ptr<JPH::BodyCreationSettings> body_settings;
    bool simulating = false;
    bool dirty = true;

    glm::vec3 starting_position = glm::vec3();
    glm::quat starting_rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
};

class RigidBodySystem : public ComponentSystem
{
public:
    void ConnectPhysicsEvents();

    void Register() override {
        ComponentRegistry::RegisterComponent<Rigidbody>(
            "RigidBody", 
            [] (rapidjson::Value& _val, rapidjson::Document::AllocatorType& _al, const Rigidbody& _t) {
                // TODO: implement
            },
            [] (const rapidjson::Value& _val, Rigidbody& _t) {
                return true;
            });
    }

private:
    void SyncTransform();

protected:
    void OnUpdate() override;
    void OnLateUpdate() override;

    void OnSimulationBegin(const Physics::OnSimulationBegin& _event);
    void OnSimulationEnd(const Physics::OnSimulationEnd& _event);
};

}