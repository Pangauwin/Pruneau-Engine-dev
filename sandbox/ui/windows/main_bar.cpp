#include "main_bar.h"

#include "level/level_manager.h"
#include "level/level_parser.h"

#include <imgui.h>

void Sandbox::DrawMainBar()
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Save"))
            {
                Core::LevelParser::SaveLevelData(*Core::LevelManager::GetCurrentLevel());
            }

            if(ImGui::MenuItem("Load"))
            {

            }

            ImGui::Separator();

            if(ImGui::MenuItem("Quit"))
            {

            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Edit"))
        {
            if(ImGui::MenuItem("Preferences"))
            {

            }

            if(ImGui::MenuItem("Settings"))
            {

            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}