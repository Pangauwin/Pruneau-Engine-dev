#pragma once

#include "core/component/component.h"
#include "core/component/component_system.h"
#include "rapidjson/document.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Body/MotionType.h>


namespace Core {

struct Rigidbody : public Component
{
    JPH::CollideShapeResult shape; // TODO: find a way to serialize this (register again)
    JPH::EMotionType motion_type = JPH::EMotionType::Dynamic;
    float mass = 1.0f;

    JPH::BodyID _body; //runtime values
    bool simulating = false;
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

protected:
    
};

}