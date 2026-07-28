#pragma once

#include "asset/asset.h"
#include "core/layer.h"
#include <variant>

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
    Asset
};

struct FocusObject {
    FocusType type = FocusType::None;
    std::uint32_t id = 0;
};

extern Core::AssetID model_id;
extern FocusObject focused_object;
}