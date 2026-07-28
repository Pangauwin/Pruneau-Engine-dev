#include "sandbox_layer.h"

#include <numbers>
#include <string>

#include "asset/asset.h"
#include "asset/asset_manager.h"
#include "components/camera.h"

#include "components/model_renderer.h"
#include "components/transform.h"
#include "core/layer.h"

#include "core/time.h"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "imgui.h"
#include "level/level.h"
#include "level/level_manager.h"

#include "components/scene_camera.h"

#include "core/application.h"
#include "renderer/renderer.h"
#include "renderer/skybox.h"

#include "ui/imgui_manager.h"
#include "ui/windows/debug_window.h"
#include "ui/windows/level_editor.h"
#include "ui/windows/properties_window.h"
#include "ui/windows/viewport.h"

#include <glad/glad.h>

static Core::Entity model = 0;
static Core::Entity cam = 0;

static float speed_multiplier = 1.0f;

namespace Sandbox {

Core::AssetID model_id = 0;
FocusObject focused_object = {.type=FocusType::None, .id=0};

SandboxLayer::SandboxLayer() : Core::Layer("SandboxLayer") {

}

void SandboxLayer::OnAttach() {
    // Component declaration
    (new Sandbox::SceneCameraSystem())->Connect();

    Sandbox::ImGuiManager::Init();

    // Asset import
    Core::AssetID shader_id = Core::AssetManager::ImportAsset("ressources/shaders/tex.vert");
    model_id = Core::AssetManager::ImportAsset("ressources/models/tralalero-tralala.fbx");
    Core::AssetID texture_id = Core::AssetManager::ImportAsset("ressources/textures/tralalero-tralala.png");

    Core::AssetID material_id = Core::AssetManager::CreateMaterial(shader_id);
    Core::AssetManager::GetAsset<Core::MaterialAsset>(material_id)
        ->SetTexture("texture_diffuse_1", texture_id);

    Core::AssetManager::GetAsset<Core::ModelAsset>(model_id)->GetMeshes()[0].materialID = material_id;

    //Level Creation
    Core::Level* lvl = new Core::Level("My level");

    Renderer::Skybox* _skybox = new Renderer::Skybox("ressources/skybox/skybox.hdr");
    Core::Application::Get()->m_renderer->m_skybox = _skybox;

    cam = lvl->CreateEntity("Camera");
    lvl->AddComponent<Core::Camera>(cam);
    lvl->AddComponent<Sandbox::SceneCamera>(cam);

    lvl->GetComponent<Core::Camera>(cam).index = 1;
    lvl->camera_index = 1;

    model = lvl->CreateEntity("Model");
    lvl->AddComponent<Core::ModelRenderer>(model);

    lvl->GetComponent<Core::ModelRenderer>(model).model_id = model_id;
    
    lvl->GetComponent<Core::Transform>(cam).position = {2.0f, 5.0f, 20.0f};

    Core::LevelManager::SwitchLevel(lvl);
}

void SandboxLayer::OnUpdate(float dt) {
    Core::Level* lvl = Core::LevelManager::GetCurrentLevel();

    Core::Transform& transform = lvl->GetComponent<Core::Transform>(model);

    transform.rotation = glm::angleAxis((float)Time::last_frame_time * speed_multiplier * 2 * (float)std::numbers::pi, transform.up);
    
    transform.dirty = true;
}

void SandboxLayer::OnGUIRender()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Sandbox::ImGuiManager::BeginFrame();

    Sandbox::DrawDebugWindow();
    Sandbox::DrawLevelEditorWindow();
    Sandbox::DrawViewport();
    Sandbox::DrawPropertiesWindow();

    ImGui::Begin("Tralalero tralala");

    ImGui::DragFloat("speed", &speed_multiplier);

    ImGui::End();

    Sandbox::ImGuiManager::EndFrame();
}

}