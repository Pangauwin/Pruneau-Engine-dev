#include "imgui_manager.h"

#include <imgui.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <ImGuizmo.h>

#include "core/application.h"

namespace Sandbox
{

ImGuiManager::ImGuiManager() {}
ImGuiManager::~ImGuiManager() {}


void ImGuiManager::Init()
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.IniFilename = "ressources/imgui.ini";

	ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(Core::Application::Get()->m_window->m_glfw_window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}


void ImGuiManager::EndFrame()
{
    ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


}