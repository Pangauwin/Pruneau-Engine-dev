#include "sandbox_layer.h"

#include "asset/asset.h"
#include "asset/asset_manager.h"
#include "components/camera.h"

#include "core/layer.h"

#include "level/level.h"
#include "level/level_manager.h"

#include "components/scene_camera.h"

#include "core/application.h"
#include "renderer/renderer.h"
#include "renderer/skybox.h"
#include "renderer/texture.h"

#include "ui/imgui_manager.h"
#include "ui/windows/debug_window.h"
#include "ui/windows/level_editor.h"
#include "ui/windows/properties_window.h"
#include "ui/windows/viewport.h"
#include "ui/windows/main_bar.h"
#include "ui/windows/asset_explorer.h"

#include <glad/glad.h>

static Core::Entity model = 0;
static Core::Entity cam = 0;

namespace Sandbox {

Core::AssetID model_id = 0;
FocusObject focused_object = {.type=FocusType::None, .id=0};
IconSet icons = {};

SandboxLayer::SandboxLayer() : Core::Layer("SandboxLayer") {

}

void SandboxLayer::OnAttach() {
    // Component declaration
    SceneCameraSystem* scene_camera_system = new Sandbox::SceneCameraSystem();
    scene_camera_system->Connect();
    scene_camera_system->Register();

    Sandbox::RegisterEngineComponentsDrawFunctions();

    Sandbox::ImGuiManager::Init();

    // Asset import
    Core::AssetID file_icon = Core::AssetManager::ImportAsset("ressources/icons/file.png");
    Core::AssetID folder_icon = Core::AssetManager::ImportAsset("ressources/icons/folder.png");

    icons.file_icon = Core::AssetManager::GetAsset<Core::TextureAsset>(file_icon)->GetTexture()->GetID();
    icons.folder_icon = Core::AssetManager::GetAsset<Core::TextureAsset>(folder_icon)->GetTexture()->GetID();

    //Level Creation
    Core::Level* lvl = new Core::Level("my-level");

    Renderer::Skybox* _skybox = new Renderer::Skybox("ressources/skybox/skybox.hdr");
    Core::Application::Get()->m_renderer->m_skybox = _skybox;

    cam = lvl->CreateEntity("Camera");
    lvl->AddComponent<Core::Camera>(cam);
    lvl->AddComponent<Sandbox::SceneCamera>(cam);

    lvl->GetComponent<Core::Camera>(cam).index = 1;
    lvl->camera_index = 1;

    Core::LevelManager::SwitchLevel(lvl);
}

void SandboxLayer::OnUpdate(float dt) {

}

void SandboxLayer::OnGUIRender()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Sandbox::ImGuiManager::BeginFrame();

    Sandbox::DrawMainBar();

    Sandbox::DrawDebugWindow();
    Sandbox::DrawLevelEditorWindow();
    Sandbox::DrawViewport();
    Sandbox::DrawPropertiesWindow();
    Sandbox::DrawAssetExplorer();

    Sandbox::ImGuiManager::EndFrame();
}

}