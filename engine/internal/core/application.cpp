#include "core/application.h"

#include <memory>

#include "platform/window.h"

#include "level/level_manager.h"

#include "core/layer.h"
#include "renderer/renderer.h"

#include "core/time.h"

#include "asset/asset_manager.h"

#include "input/input.h"

#include "components/transform.h"
#include "components/camera.h"

static float dt = 0.00001f; // Avoid 0 divisions errors

static Core::Application* current_application;


Core::Application::Application(AppParams _params) : 
	m_window(std::make_unique<Platform::Window>(_params.window_params)), 
	m_renderer(std::make_unique<Renderer::Renderer>(m_window.get())),
	m_app_should_close(false)
{
	current_application = this;
}

Core::Application::~Application()
{

}

void Core::Application::Init()
{
	AssetManager::Init();
	Time::Init();

	LevelManager::Init();

	TransformSystem* transform_system = new TransformSystem(); // TODO : Make this in a proper scope, destroy them at the end

	transform_system->Register();
	transform_system->Connect();

	(new CameraSystem())->Connect();

	for(Layer* l: m_layer_stack)
	{
		l->OnAttach();
	}
}

void Core::Application::Run()
{
	Init();

	while (!m_app_should_close)
	{
		Time::Update();
		Input::UpdateMousePosition();
		Input::UpdateMouseDelta();

		PollEvents();
		m_window->SwapBuffers();

		dt = Time::delta_time;

		for (Layer* _layer : m_layer_stack)
			_layer->OnUpdate(dt);

		LevelManager::OnUpdate(dt);

		LevelManager::OnLateUpdate(dt);

		m_renderer->PreRender();

		LevelManager::OnRender();

		for (Layer* _layer : m_layer_stack)
			_layer->OnRender();

		m_renderer->PostRender();

		m_renderer->PreGUIRender();

		for (Layer* _layer : m_layer_stack)
			_layer->OnGUIRender();

		LevelManager::OnGUIRender();

		for(Layer* _layer : m_layer_stack)
			_layer->PostGUIRender();

		m_renderer->PostGUIRender();
	}

	OnClose();
}

void Core::Application::OnEvent(Event& _event)
{
	/*
	Exemple of event dispatching (put this in the layer code)

	#include "core/event/event_dispatcher.h"

	EventDispatcher dispatcher(_event);

	dispatcher.Dispatch<Platform::WindowCloseEvent>([this](Platform::WindowCloseEvent&) {
		m_app_should_close = true;
	});*/

	for (auto it = m_layer_stack.end(); it != m_layer_stack.begin();)
	{
		(*--it)->OnEvent(_event);
		if (_event.handled)
			break;
	}
}

void Core::Application::CloseApplication()
{
	m_app_should_close = true;
}

void Core::Application::PushLayer(Layer* _layer)
{
	m_layer_stack.PushLayer(_layer);
}

void Core::Application::PushOverlay(Layer* _layer)
{
	m_layer_stack.PushOverlay(_layer);
}

Core::Application* Core::Application::Get()
{
	return current_application;
}

void Core::Application::PollEvents()
{
	m_window->PollEvents();
}

void Core::Application::OnClose()
{
	for (Layer* _layer : m_layer_stack)
		_layer->OnDetach();
}
