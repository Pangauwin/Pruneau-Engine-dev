#pragma once

#include "asset/asset.h"
#include "core/component.h"

#include "renderer/model.h"

#include "renderer/renderer.h"

#include <memory>

namespace Core
{

class ModelRenderer : public Component, AutoRegisterComponent<ModelRenderer>
{
public:
	explicit ModelRenderer(Entity* owner) : Component(owner), model(nullptr), m_model_id(0) // Used by editor
	{
		Renderer::Renderer::Get()->RegisterModel(this);
	} 

	ModelRenderer(Entity* owner, AssetID _model); // Used for compiler to distinguish the one used by editor and the normal one
	~ModelRenderer();

	void SetModelID(AssetID _model_id);

	std::shared_ptr<Renderer::Model> model;
	AssetID m_model_id;
private:

};

}