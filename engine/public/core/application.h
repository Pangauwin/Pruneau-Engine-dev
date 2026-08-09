#pragma once

#include <string>
#include <memory>


#include "../platform/window.h"
#include "../renderer/renderer.h"
#include "layer_stack.h"

#include "physics/physics_engine.h"

namespace Core {

struct AppParams {
	Platform::WindowParams window_params = {};
	Renderer::RendererConfig renderer_config = {};
};

void LogMessage(std::string _message);
void LogMessageDebug(std::string _message);
void LogMessageInfo(std::string _message);
void LogMessageWarning(std::string _message);
void LogMessageError(std::string _message);

class Application {
public:
	Application(AppParams _params);
	~Application();

	void Run();
	void OnEvent(Event& _event);
	void CloseApplication();

	void PushLayer(Layer* _layer);
	void PushOverlay(Layer* _layer);

	static Application* Get();

private:
	void PollEvents();
	void Init();

private:
	void OnClose();

public:
	std::unique_ptr<Platform::Window> m_window;
	std::unique_ptr<Renderer::Renderer> m_renderer;

	std::unique_ptr<Physics::PhysicsEngine> m_physics_engine;

private:
	bool m_app_should_close;

	LayerStack m_layer_stack;
};

}