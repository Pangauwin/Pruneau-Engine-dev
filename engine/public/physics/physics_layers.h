#pragma once

#include <Jolt/Jolt.h>

#include "Jolt/Core/IssueReporting.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Physics::Layers {

static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYER = 2;

namespace BroadPhaseLayers {
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr uint NUM_LAYER(2);
}


class BroadPhaseLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    virtual uint GetNumBroadPhaseLayers() const override
    {
        return NUM_LAYER;
    }


    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(
        JPH::ObjectLayer layer) const override
    {
        if(layer == Layers::MOVING)
            return JPH::BroadPhaseLayer(MOVING);

        return JPH::BroadPhaseLayer(NON_MOVING);
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override
    {
        switch ((JPH::BroadPhaseLayer::Type)layer)
        {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:     return "MOVING";
        default: return "INVALID";
        }
    }
#endif
};

class ObjectLayerPairFilterImpl final : public JPH::ObjectLayerPairFilter
{
public:
    bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override
    {
        if(a == NON_MOVING && b == NON_MOVING)
            return false;
        return true; //TODO : When sensors implemented, check if one of these are sensor, then good
    }
};

class ObjectVsClassBroadPhaseLayerImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    bool ShouldCollide(JPH::ObjectLayer layer_1, JPH::BroadPhaseLayer layer_2) const override
    {
        switch (layer_1) {
            case NON_MOVING:
                return layer_2 == BroadPhaseLayers::MOVING;
            case MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
};

}