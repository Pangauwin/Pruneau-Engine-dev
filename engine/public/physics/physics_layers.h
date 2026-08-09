#pragma once

#include <Jolt/Jolt.h>

#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Physics::Layers {

static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYER = 2;


class BroadPhaseLayerInterfaceImpl : public JPH::BroadPhaseLayerInterface
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
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override
    {
        return true; //TODO : When sensors implemented, check if one of these are sensor, then good
    }
};

}