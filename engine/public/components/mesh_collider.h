#pragma once

#include "asset/asset.h"
#include "core/component/component.h"
#include "core/component/component_system.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace Core
{

enum ColliderShapeType { ConcaveMesh, ConvexMesh } ;

struct MeshCollider : Component
{
    AssetID mesh_asset_id = 0;
    ColliderShapeType shape_type = ColliderShapeType::ConcaveMesh;

    bool dirty = true;
};

class MeshColliderSystem: public ComponentSystem
{
public:
	void Register() override
	{
		ComponentRegistry::RegisterComponent<MeshCollider>(
			"Mesh Collider", 
			[] (rapidjson::Value& _val, rapidjson::Document::AllocatorType& _al, const MeshCollider& _t)  
			{
				_val.AddMember("mesh_id", _t.mesh_asset_id, _al);
			},
			[] (const rapidjson::Value& _val, MeshCollider& _t)
			{
				if(!_val.HasMember("mesh_id")) return false;

				_t.mesh_asset_id = _val["mesh_id"].GetUint();
				return true;
			});
	}
};

}