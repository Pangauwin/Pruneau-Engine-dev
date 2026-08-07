#pragma once

#include "asset/asset.h"
#include "core/layer.h"

#include <imgui.h>

namespace Core
{
    using Entity = std::uint32_t;
    using AssetID = std::uint32_t;
}

namespace Sandbox
{

class SandboxLayer: public Core::Layer
{
public:
    SandboxLayer();

    void OnAttach() override;
    void OnUpdate(float dt) override;
    void OnGUIRender() override;
};

enum class FocusType{
    None,
    Entity,
    Asset,
    Folder
};

struct FocusObject {
    FocusType type = FocusType::None;
    std::uint32_t id = 0;
};

struct IconSet{
    ImTextureID folder_icon = 0;
    ImTextureID file_icon = 0;
};

extern Core::AssetID model_id;
extern FocusObject focused_object;
extern IconSet icons;
}