#include "main_bar.h"

#include "level/level.h"
#include "level/level_manager.h"
#include "level/level_parser.h"

#include <imgui.h>

static std::filesystem::path level_data_file_location = "/home/jerome-gauwin/Documents/dev/Pruneau-Engine-dev/ressources/saves/my-level.plvl";

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
                Core::Level* _new_level = Core::LevelParser::LoadLevelData(level_data_file_location);
                Core::LevelManager::SwitchLevel(_new_level);
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