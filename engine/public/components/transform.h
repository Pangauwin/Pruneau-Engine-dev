#pragma once

#include "core/component/component.h"
#include "core/component/component_system.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "level/level.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
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
			[] (rapidjson::Value& _val, rapidjson::Document::AllocatorType& _al, const Transform& t)
			{
				rapidjson::Value position(rapidjson::kObjectType);
				rapidjson::Value rotation(rapidjson::kObjectType);
				rapidjson::Value scale(rapidjson::kObjectType);

				save(position, _al, t.position);
				save(rotation, _al, t.rotation);
				save(scale, _al, t.scale);

				_val.AddMember("Position", position, _al);
				_val.AddMember("Rotation", rotation, _al);
				_val.AddMember("Scale", scale, _al);
			},
			[] (const rapidjson::Value& _val, Transform& t)
			{
				if(!(_val.HasMember("Position") && 
					_val.HasMember("Rotation") && 
					_val.HasMember("Scale"))) return false;

				if(!(_val["Position"].IsNumber() && 
					_val["Rotation"].IsNumber() && 
					_val["Scale"].IsNumber())) return false;

				load(_val["Position"], t.position);
				load(_val["Rotation"], t.rotation);
				load(_val["Scale"], t.scale);

				t.dirty = true;
				t.world_transform = glm::mat4(1.f);
				t.local_transform = glm::mat4(1.f);

				t.forward = glm::vec3{};
				t.up      = glm::vec3{};
				t.right   = glm::vec3{};

				return true;
			}
		);
	}

protected:
	void OnUpdate() override;
};
}