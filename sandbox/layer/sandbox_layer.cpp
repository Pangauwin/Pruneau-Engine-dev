#include "sandbox_layer.h"

#include <string>

#include "asset/asset.h"
#include "asset/asset_manager.h"
#include "components/camera.h"

#include "components/model_renderer.h"
#include "components/transform.h"
#include "core/layer.h"

#include "core/time.h"
#include "level/level.h"
#include "level/level_manager.h"

#include "components/scene_camera.h"

#include "core/application.h"
#include "renderer/skybox.h"

static Core::Entity cube = 0;
static Core::Entity cam = 0;

SandboxLayer::SandboxLayer() : Core::Layer("SandboxLayer") {

}

void SandboxLayer::OnAttach() {
    // Component declaration
    (new Sandbox::SceneCameraSystem())->Connect();

    // Asset import
    Core::AssetID shader_id = Core::AssetManager::ImportAsset("ressources/shaders/tex.vert");
    Core::AssetID cube_id = Core::AssetManager::ImportAsset("ressources/models/cube.fbx");
    Core::AssetID texture_id = Core::AssetManager::ImportAsset("ressources/textures/texture.png");

    Core::AssetID material_id = Core::AssetManager::CreateMaterial(shader_id);
    Core::AssetManager::GetAsset<Core::MaterialAsset>(material_id)
        ->SetTexture("texture_diffuse_1", texture_id);

    Core::AssetManager::GetAsset<Core::ModelAsset>(cube_id)->GetMeshes()[0].materialID = material_id;

    //Level Creation
    Core::Level* lvl = new Core::Level("My level");

    Renderer::Skybox* _skybox = new Renderer::Skybox("ressources/skybox/skybox.hdr");
    Core::Application::Get()->m_renderer->m_skybox = _skybox;

    Core::Entity ground = lvl->CreateEntity("Ground");
    lvl->AddComponent<Core::ModelRenderer>(ground);
    lvl->GetComponent<Core::ModelRenderer>(ground).model_id = cube_id;
    lvl->GetComponent<Core::Transform>(ground).scale = {100.f, 1.0f, 100.f};

    cam = lvl->CreateEntity("Camera");
    lvl->AddComponent<Core::Camera>(cam);
    lvl->AddComponent<Sandbox::SceneCamera>(cam);

    lvl->GetComponent<Core::Camera>(cam).index = 1;
    lvl->camera_index = 1;

    cube = lvl->CreateEntity("Cube");
    lvl->AddComponent<Core::ModelRenderer>(cube);
    lvl->GetComponent<Core::Transform>(cube).position = {0.0f, 2.0f, 0.0f};

    lvl->GetComponent<Core::ModelRenderer>(cube).model_id = cube_id;
    
    lvl->GetComponent<Core::Transform>(cam).position = {2.0f, 5.0f, 10.0f};

    Core::LevelManager::SwitchLevel(lvl);
}

void SandboxLayer::OnUpdate(float dt)
{
    Core::Level* lvl = Core::LevelManager::GetCurrentLevel();

    Core::Transform& transform = lvl->GetComponent<Core::Transform>(cube);
    
    transform.position.x = sin(Time::last_frame_time) * 3.0f;
    transform.dirty = true;
}