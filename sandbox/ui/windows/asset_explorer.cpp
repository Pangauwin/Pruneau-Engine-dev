#include "asset_explorer.h"
#include "asset/asset.h"
#include "asset/asset_manager.h"

#include "layer/sandbox_layer.h"

#include "renderer/texture.h"

#include <imgui.h>

static Core::FolderID s_current_folder = 0;

void Sandbox::DrawAssetExplorer()
{
    ImGui::Begin("Asset Explorer");

    Core::AssetFolder& _current_folder = Core::AssetManager::GetFolder(s_current_folder);

    if(ImGui::Button("..")) s_current_folder = _current_folder.parent;

    if(ImGui::BeginTable("Explorer", 3, ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Sortable |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("AssetID");

        for(auto& _folder: _current_folder.children)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Image(icons.folder_icon, ImVec2(32, 32));
        
            ImGui::TableSetColumnIndex(1);
            ImGuiSelectableFlags flags =
                ImGuiSelectableFlags_SpanAllColumns |
                ImGuiSelectableFlags_AllowOverlap;
            
            ImGui::PushID(_folder->id);

            if(ImGui::Selectable(_folder->name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0.0f, 32.0f)))
            {
                focused_object.type = FocusType::Folder;
                focused_object.id = _folder->id;
            }

            if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                s_current_folder = _folder->id;
            }

            ImGui::PopID();

            ImGui::TableSetColumnIndex(2);

            ImGui::TextUnformatted(std::to_string(_folder->id).c_str());
        }

        for (std::shared_ptr<Core::Asset> _asset : _current_folder.assets) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);

            ImGui::PushID(_asset.get());

            if(std::shared_ptr<Core::TextureAsset> _tex = Core::AssetManager::GetAsset<Core::TextureAsset>(_asset->GetID()))
                ImGui::Image(_tex->GetTexture()->GetID(), ImVec2(32, 32));
            else
                ImGui::Image(icons.file_icon, ImVec2(32, 32));
        
            ImGui::TableSetColumnIndex(1);
            ImGuiSelectableFlags flags =
                ImGuiSelectableFlags_SpanAllColumns |
                ImGuiSelectableFlags_AllowOverlap;
            
            if(ImGui::Selectable(_asset->name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0.0f, 32.0f)))
            {
                focused_object.type = FocusType::Asset;
                focused_object.id = _asset->GetID();
            }

            ImGui::PopID();

            if(ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("AssetContextMenu");
            }

            if(ImGui::BeginPopupContextItem())
            {
                // Editor::AssetDisplayer::DrawComponentContextMenu(*_asset.get());
                ImGui::MenuItem("Remove"); // TODO : Implement and reimplement the contextmenu fct (later)

                ImGui::EndPopup();
            }

            ImGui::TableSetColumnIndex(2);

            ImGui::TextUnformatted(std::to_string(_asset->GetID()).c_str());
        }
        ImGui::EndTable();
    }

    ImGui::End();
}