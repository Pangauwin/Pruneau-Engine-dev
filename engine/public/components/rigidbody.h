#pragma once

#include "core/component/component.h"
#include "core/component/component_system.h"
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
};

class RigidBodySystem : public ComponentSystem
{
public:
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
};

}