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
    if(!Renderer::Renderer::Get())
    {
        Core::LogMessageError("No Renderer Detected, Model registration impossible !");
        return;
    }
    Renderer::Renderer::Get()->RegisterModel(this);
}

ModelRenderer::~ModelRenderer()
{
    if(!Renderer::Renderer::Get())
    {
        Core::LogMessageError("Renderer not detected ! ModelRenderer unregistration impossible !");
        return;
    }
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
