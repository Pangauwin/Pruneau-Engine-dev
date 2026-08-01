#include "core/component/component.h"

std::unordered_map<entt::id_type, Core::ComponentInfo> Core::ComponentRegistry::m_component_registry;
std::unordered_map<std::string, entt::id_type> Core::ComponentRegistry::m_name_to_id;