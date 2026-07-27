#pragma once

#include "core/layer.h"

class SandboxLayer: public Core::Layer
{
public:
    SandboxLayer();

    void OnAttach() override;
    void OnUpdate(float dt) override;
    void OnGUIRender() override;
};