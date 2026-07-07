#include "asset_displayer.h"
#include "asset/asset.h"

#include <stdlib.h>
#include <cstdint>
#include <cstring>

#include <functional>
#include <typeindex>
#include <unordered_map>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <renderer/texture.h>
#include <asset/asset_manager.h>

#include <renderer/model.h>
#include <glm/glm.hpp>
#include <renderer/mesh.h>


using InspectorFunction = std::function<void(Core::Asset&)>;
static std::unordered_map<std::type_index, InspectorFunction> s_inspector_functions;

using ContextMenuFunction = std::function<void(Core::Asset&)>;
static std::unordered_map<std::type_index, ContextMenuFunction> s_context_menu_functions;

static bool registered = false;

void RegisterAssetDrawFunctions();
void RegisterAssetContextMenuFunctions();

void DrawDefaultInspectorFunction(Core::Asset &_asset);
void DrawDefaultContextMenuFunction(Core::Asset &_asset);

void Editor::AssetDisplayer::DrawComponentGui(Core::Asset &_asset)
{
    if(!registered)
    {
        RegisterAssetContextMenuFunctions();
        RegisterAssetDrawFunctions();

        registered = true;
    }

    ImGui::InputText("Asset Name", &_asset.name);

    ImGui::Text("Asset ID: %s", std::to_string(_asset.GetID()).c_str());

    auto it = s_inspector_functions.find(typeid(_asset));

    if(it == s_inspector_functions.end())
    {
        DrawDefaultInspectorFunction(_asset);
        return;
    }

    it->second(_asset);
}

void Editor::AssetDisplayer::DrawComponentContextMenu(Core::Asset &_asset)
{
    if(!registered)
    {
        RegisterAssetContextMenuFunctions();
        RegisterAssetDrawFunctions();

        registered = true;
    }
    
    auto it = s_context_menu_functions.find(typeid(_asset));

    if(it == s_context_menu_functions.end())
    {
        DrawDefaultContextMenuFunction(_asset);
        return;
    }

    it->second(_asset);
}

void RegisterAssetDrawFunctions(){
    s_inspector_functions[typeid(Core::TextureAsset)] = [](Core::Asset& _asset)
    {
        Core::TextureAsset& texture_asset = static_cast<Core::TextureAsset&>(_asset);

        ImGui::Text("Texture preview:");
        Renderer::Texture* tex = texture_asset.GetTexture();
        if(tex)
            ImGui::Image((void*)(intptr_t)tex->GetID(), ImVec2(tex->GetWidth(), tex->GetHeight()));
        else
            ImGui::Text("No render available (texture may be broken)");
    };

    s_inspector_functions[typeid(Core::ModelAsset)] = [](Core::Asset& _asset)
    {
        Core::ModelAsset& model_asset = static_cast<Core::ModelAsset&>(_asset);

        Renderer::Model* m_model = model_asset.GetModel().get();

        ImGui::Text("Internal meshes ids: ");
        for (auto& _mesh : m_model->GetMeshes())
        {
            std::shared_ptr<Core::MeshAsset> _asset = std::get<std::shared_ptr<Core::MeshAsset>>(_mesh);
            ImGui::BulletText("%s", _asset->GetName().c_str());

            int id = _asset->GetMesh()->m_material->GetID();

            ImGui::PushID(_asset.get());

            if(ImGui::DragInt("MaterialID", &id))
            {
                if(std::shared_ptr<Core::MaterialAsset> _mat = Core::AssetManager::GetAsset<Core::MaterialAsset>(id))
                {
                    _asset->GetMesh()->m_material = _mat;
                }
            }

            ImGui::PopID();
        }
    };

    s_inspector_functions[typeid(Core::MaterialAsset)] = [](Core::Asset& _asset)
    {
        Core::MaterialAsset& material_asset = static_cast<Core::MaterialAsset&>(_asset);

        if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& [uniform_name, tex] : material_asset.m_textures)
        {
            ImGui::Text("%s:", uniform_name.c_str());
            
			int v = 0;

			if(tex)
				v = tex->GetID();
			
			if(ImGui::DragInt("TextureID: ", &v))
			{
				tex = Core::AssetManager::GetAsset<Core::TextureAsset>(v);
			}
			if(tex)
			{
				Renderer::Texture* texture = tex->GetTexture();
				if (texture)
				{
					ImGui::Image((void*)(intptr_t)texture->GetID(), ImVec2(64,64));
				}
			}
        }

		if(ImGui::Button("Add Texture"))
			ImGui::OpenPopup("Add Texture Popup");

		if(ImGui::BeginPopup("Add Texture Popup"))
		{
			static char name[64] = "";
			static int texture_id = 0;

			ImGui::InputText("Name", name, 64);
			ImGui::DragInt("Texture ID", &texture_id);

			if(ImGui::Button("Add"))
			{
				if(strlen(name) > 0)
				{
					std::shared_ptr<Core::TextureAsset> texture = Core::AssetManager::GetAsset<Core::TextureAsset>(texture_id);
					material_asset.m_textures[name] = texture;

					name[0] = '\0';
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
    }

	if (ImGui::CollapsingHeader("Uniforms", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto it = material_asset.m_uniforms.begin(); it != material_asset.m_uniforms.end(); )
		{
			auto& [name, value] = *it;

			ImGui::PushID(name.c_str());

			ImGui::Text("%s", name.c_str());
			ImGui::SameLine();

			bool remove = false;

			std::visit([&](auto& arg)
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, int>)
					ImGui::DragInt("##v", &arg);

				else if constexpr (std::is_same_v<T, float>)
					ImGui::DragFloat("##v", &arg, 0.1f);

				else if constexpr (std::is_same_v<T, glm::vec2>)
					ImGui::DragFloat2("##v", &arg.x, 0.1f);

				else if constexpr (std::is_same_v<T, glm::vec3>)
					ImGui::ColorEdit3("##v", &arg.x);

				else if constexpr (std::is_same_v<T, glm::vec4>)
					ImGui::ColorEdit4("##v", &arg.x);

				else if constexpr (std::is_same_v<T, glm::mat4>)
				{
					ImGui::Text("mat4");
				}

			}, value);

			ImGui::SameLine();

			if (ImGui::Button("X"))
				remove = true;

			ImGui::PopID();

			if (remove)
				it = material_asset.m_uniforms.erase(it);
			else
				++it;
		}

		if (ImGui::Button("Add Uniform"))
			ImGui::OpenPopup("AddUniformPopup");

		if (ImGui::BeginPopup("AddUniformPopup"))
		{
			static char name[64] = "";
			static int type = 0;

			const char* types[] = { "int", "float", "vec2", "vec3", "vec4" };

			ImGui::InputText("Name", name, 64);
			ImGui::Combo("Type", &type, types, IM_ARRAYSIZE(types));

			if (ImGui::Button("Create"))
			{
				if (strlen(name) > 0 && material_asset.m_uniforms.find(name) == material_asset.m_uniforms.end())
				{
					switch (type)
					{
						case 0: material_asset.m_uniforms[name] = 0; break;
						case 1: material_asset.m_uniforms[name] = 0.0f; break;
						case 2: material_asset.m_uniforms[name] = glm::vec2(0.0f); break;
						case 3: material_asset.m_uniforms[name] = glm::vec3(1.0f); break;
						case 4: material_asset.m_uniforms[name] = glm::vec4(1.0f); break;
					}

					name[0] = '\0';
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
    };

    
}

void RegisterAssetContextMenuFunctions()
{
    s_context_menu_functions[typeid(Core::ShaderAsset)] = [](Core::Asset& _asset)
    {
        Core::ShaderAsset& shader_asset = static_cast<Core::ShaderAsset&>(_asset);

        if(ImGui::MenuItem("Create Material from Shader"))
        {
            Core::AssetManager::CreateMaterial(Core::AssetManager::GetAsset<Core::ShaderAsset>(shader_asset.GetID()));
        }
    };
}


void DrawDefaultInspectorFunction(Core::Asset &_asset)
{

}

void DrawDefaultContextMenuFunction(Core::Asset &_asset)
{

}