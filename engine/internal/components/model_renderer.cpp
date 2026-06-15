#include "components/model_renderer.h"

#include "asset/asset.h"
#include "renderer/renderer.h"

#include "asset/asset_manager.h"

#include "core/log/log.h"
#include "core/entity.h"

namespace Core {

static bool registered = []()
    {
        AutoRegisterComponent<ModelRenderer>::Register("Model Renderer");
        return true;
    }();

ModelRenderer::ModelRenderer(Entity* _owner, AssetID _model) : Component(_owner), m_model_id(_model), model(nullptr)
{
    if (auto model_asset = AssetManager::GetAsset<ModelAsset>(m_model_id))
    {
        model = model_asset->GetModel();
    }
    Renderer::Renderer::Get()->RegisterModel(this);
}

ModelRenderer::~ModelRenderer()
{
    Renderer::Renderer::Get()->UnRegisterModel(this);
}

void ModelRenderer::SetModelID(Core::AssetID _model_id)
{
    if(auto model_asset = AssetManager::GetAsset<ModelAsset>(_model_id))
    {
        m_model_id = _model_id;
        model = model_asset->GetModel();
    }
    else {
        Core::LogMessageWarning("Object " + GetOwner()->name + " ");
    }
}
}
