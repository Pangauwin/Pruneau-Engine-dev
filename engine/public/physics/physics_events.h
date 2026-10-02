#pragma once

#include "entt/entity/fwd.hpp"
#include "glm/ext/vector_float3.hpp"
#include <entt/fwd.hpp>


namespace Physics
{

struct OnSimulationBegin { entt::entity _ent; };
struct OnSimulationEnd { entt::entity _ent; };
struct AddForce {entt::entity _ent; glm::vec3 _force;};

}