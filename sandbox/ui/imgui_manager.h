#pragma once

namespace Sandbox
{

class ImGuiManager
{
public:
    ImGuiManager();
    ~ImGuiManager();

    static void Init();

    static void BeginFrame();

    static void EndFrame();
};

}