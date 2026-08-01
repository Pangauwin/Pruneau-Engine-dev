#pragma once

#include <cereal/cereal.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace glm
{

template<class Archive>
void serialize(Archive& ar, vec3& v)
{
    ar(
        cereal::make_nvp("x", v.x),
        cereal::make_nvp("y", v.y),
        cereal::make_nvp("z", v.z)
    );
}

template<class Archive>
void serialize(Archive& ar, quat& q)
{
    ar(
        cereal::make_nvp("w", q.w),
        cereal::make_nvp("x", q.x),
        cereal::make_nvp("y", q.y),
        cereal::make_nvp("z", q.z)
    );
}

}