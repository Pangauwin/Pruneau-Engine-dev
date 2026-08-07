#pragma once

#include <entt/entt.hpp>

#include <imgui.h>
#include <string>
#include <vector>
#include <functional>

namespace Sandbox
{
    void DrawPropertiesWindow();

    extern std::vector<std::function<void(entt::registry& _reg, entt::entity _ent)>> draw_functions;;

    template<typename T>
    void RegisterDrawFunctionComponent(std::string name, std::function<void(T& comp)> _draw)
    {
        std::function<void(entt::registry& _reg, entt::entity _ent)> draw_function = [_draw, name] (entt::registry& _reg, entt::entity _ent)
        {
            if(_reg.all_of<T>(_ent))
            {
                T& _comp = _reg.get<T>(_ent);

                if(ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    _draw(_comp);
                }
            }
        };

        draw_functions.push_back(draw_function);
    }

    void RegisterEngineComponentsDrawFunctions();
}