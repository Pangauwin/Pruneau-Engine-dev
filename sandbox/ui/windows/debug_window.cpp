#include "debug_window.h"

#include <imgui.h>
#include "core/time.h"
#include <entt/entt.hpp>
#include "level/level_manager.h"
#include "components/transform.h"
#include "components/model_renderer.h"

void Sandbox::DrawDebugWindow()
{
    ImGui::Begin("Debug Window");

    ImGui::Text("FPS: %.1f", Time::frame_per_second);
    ImGui::Text("Delta time: %.4fs", Time::delta_time);

    unsigned int entity_count = Core::LevelManager::GetCurrentLevel()->GetRegistry().view<Core::Transform>()->size();

    ImGui::Text("Entity count: %i", entity_count);

    unsigned int model_count = Core::LevelManager::GetCurrentLevel()->GetRegistry().view<Core::ModelRenderer>()->size();

    ImGui::Text("Model count: %i", model_count);

    ImGui::End();
}