#include "sandbox_layer.h"

#include <entt/entt.hpp>
#include <memory>

#include "asset/asset.h"
#include "asset/asset_manager.h"
#include "components/camera.h"

#include "components/model_renderer.h"
#include "components/transform.h"
#include "core/layer.h"

#include "entt/entity/fwd.hpp"
#include "level/level.h"
#include "level/level_manager.h"

#include "renderer/mesh.h"
#include "renderer/model.h"


SandboxLayer::SandboxLayer() : Core::Layer("SandboxLayer") {

}

void SandboxLayer::OnAttach() {
    // Asset import
    Core::AssetID shader_id = Core::AssetManager::ImportAsset("ressources/shaders/tex.vert");
    Core::AssetID cube_id = Core::AssetManager::ImportAsset("ressources/models/cube.fbx");
    Core::AssetID texture_id = Core::AssetManager::ImportAsset("ressources/textures/texture.png");

    Core::AssetID material_id = Core::AssetManager::CreateMaterial(Core::AssetManager::GetAsset<Core::ShaderAsset>(shader_id));
    Core::AssetManager::GetAsset<Core::MaterialAsset>(material_id)->m_textures["texture_diffuse_1"] = Core::AssetManager::GetAsset<Core::TextureAsset>(texture_id);

    std::get<std::shared_ptr<Core::MeshAsset>>(Core::AssetManager::GetAsset<Core::ModelAsset>(cube_id)->GetModel().get()->GetMeshes()[0])->GetMesh()->m_material =
        Core::AssetManager::GetAsset<Core::MaterialAsset>(material_id);

    //Level Creation
    Core::Level* lvl = new Core::Level("My level");

    Core::Entity cam = lvl->CreateEntity("Camera");
    lvl->AddComponent<Core::Camera>(cam);
    lvl->AddComponent<Core::Transform>(cam);

    lvl->GetRegistry().get<Core::Camera>(static_cast<entt::entity>(cam)).index = 1;
    lvl->camera_index = 1;

    Core::Entity cube = lvl->CreateEntity("Cube");
    lvl->AddComponent<Core::ModelRenderer>(cube);
    lvl->AddComponent<Core::Transform>(cube);
    
    lvl->GetRegistry().get<Core::ModelRenderer>(static_cast<entt::entity>(cube)).model_id = cube_id;
    lvl->GetRegistry().get<Core::Transform>(static_cast<entt::entity>(cam)).position = {2.0f, 5.0f, 10.0f};
    lvl->GetRegistry().get<Core::Transform>(static_cast<entt::entity>(cam)).dirty = true;

    Core::LevelManager::SwitchLevel(lvl);
}