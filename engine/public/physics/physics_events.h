#pragma once

#include <entt/fwd.hpp>

namespace Physics
{

struct OnSimulationBegin { entt::entity _ent; };
struct OnSimulationEnd { entt::entity _ent; };

}