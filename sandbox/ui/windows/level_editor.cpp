#include "level_editor.h"

#include <imgui.h>

#include "components/model_renderer.h"
#include "components/transform.h"
#include "entt/entity/entity.hpp"
#include "level/level.h"
#include "level/level_manager.h"

#include <entt/entt.hpp>
#include <string>

#include "layer/sandbox_layer.h"

void Sandbox::DrawLevelEditorWindow()
{
    ImGui::Begin("Level Editor");

    Core::Level* _level = Core::LevelManager::GetCurrentLevel();
    entt::registry& _registry = _level->GetRegistry();

    static int selected_item = -1;


    if(ImGui::Button("+"))
    {
        _level->CreateEntity("Entity");
        ++selected_item;
    }

    ImGui::SameLine();

    if(ImGui::Button("+ model"))
    {
        Core::Entity _model = _level->CreateEntity("Model");
        _level->AddComponent<Core::ModelRenderer>(_model);
        _level->GetComponent<Core::ModelRenderer>(_model).model_id = Sandbox::model_id;
        if(selected_item != -1)
            ++selected_item;
    }

    ImGui::Spacing();

    ImGui::Text("Entities: ");

    auto view = _registry.view<Core::Transform>();

    size_t size = view.size();

    const char * items[size];

    unsigned int i = 0;
    for(auto& ent: view)
    {
        items[i] = _level->GetComponent<Core::Entity_info>(ent).name.c_str();
        ++i;
    }

    int previous_selected = selected_item;

    if(selected_item >= IM_ARRAYSIZE(items))
    {
        selected_item = -1;
    }

    ImGui::ListBox("##entities", &selected_item, 
        items, IM_ARRAYSIZE(items));

    if(selected_item != previous_selected && selected_item >= 0)
    {
        focused_object.type = FocusType::Entity;

        auto it = view.begin();
        std::advance(it, selected_item);

        entt::entity entity = *it;
        focused_object.id = entt::to_integral(entity);
    }
    ImGui::End();
}