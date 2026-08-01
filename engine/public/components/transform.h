#pragma once

#include "cereal/archives/json.hpp"
#include "cereal/cereal.hpp"
#include <cereal/types/vector.hpp>
#include "core/component/component.h"
#include "core/component/component_system.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "level/level.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "serialization/types_serializer.h"

namespace Core
{

struct Transform : public Component
{
	bool dirty = true;
	glm::mat4 world_transform = glm::mat4(1.0f);
	glm::mat4 local_transform = glm::mat4(1.0f);

	glm::vec3 position = glm::vec3();
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3({1, 1, 1});

	glm::vec3 forward = glm::vec3();
	glm::vec3 up = glm::vec3();
	glm::vec3 right = glm::vec3();

	Entity parent = 0;
	std::vector<Entity> children;
};

class TransformSystem: public ComponentSystem
{
private:
	void UpdateMatrix(Core::Transform& _transform);

public:
	void Register() override {
		ComponentRegistry::RegisterComponent<Transform>(
			"Transform",
			[] (SaveArchive& ar, const Transform& t)
			{
				ar(
					cereal::make_nvp("Position", t.position),
					cereal::make_nvp("Rotation", t.rotation),
					cereal::make_nvp("Scale",    t.scale),
					cereal::make_nvp("Parent",   t.parent),
					cereal::make_nvp("Children", t.children)
				);
			},
			[] (LoadArchive& ar, Transform& t)
			{
				ar(
					cereal::make_nvp("Position", t.position),
					cereal::make_nvp("Rotation", t.rotation),
					cereal::make_nvp("Scale",    t.scale),
					cereal::make_nvp("Parent",   t.parent),
					cereal::make_nvp("Children", t.children)
				);

				t.dirty = true;
				t.world_transform = glm::mat4(1.f);
				t.local_transform = glm::mat4(1.f);

				t.forward = glm::vec3{};
				t.up      = glm::vec3{};
				t.right   = glm::vec3{};
			}
		);
	}

protected:
	void OnUpdate() override;
};
}