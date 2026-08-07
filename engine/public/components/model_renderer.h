#pragma once

#include "asset/asset.h"
#include "core/component/component.h"
#include "core/component/component_system.h"

namespace Core
{

struct ModelRenderer : public Component
{
	AssetID model_id = 0;
};

class ModelRendererSystem: public ComponentSystem
{
public:
	void Register() override
	{
		ComponentRegistry::RegisterComponent<ModelRenderer>(
			"Model Renderer", 
			[] (rapidjson::Value& _val, rapidjson::Document::AllocatorType& _al, const ModelRenderer& _t)  
			{
				_val.AddMember("model_id", _t.model_id, _al);
			},
			[] (const rapidjson::Value& _val, ModelRenderer& _t)
			{
				if(!_val.HasMember("model_id")) return false;

				_t.model_id = _val["model_id"].GetUint();
				return true;
			});
	}
};
}