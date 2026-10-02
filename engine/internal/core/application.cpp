#include "core/application.h"

#include <memory>

#include "components/mesh_collider.h"
#include "components/rigidbody.h"
#include "physics/physics_world.h"
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

static float accumulator = 0.0f;
constexpr float FIXED_DT = 1.0f / 60.0f; // 60Hz physics update
constexpr float MAX_ACCUMULATED = 0.25f;

Core::Application::Application(AppParams _params) : 
	m_window(std::make_unique<Platform::Window>(_params.window_params)), 
	m_renderer(std::make_unique<Renderer::Renderer>(m_window.get(), _params.renderer_config)),
	m_app_should_close(false), m_physics_engine(std::make_unique<Physics::PhysicsWorld>()),
	m_transform_system(nullptr), m_camera_system(nullptr), m_rigidbody_system(nullptr), m_meshcollider_system(nullptr)
{
	current_application = this;
}

Core::Application::~Application()
{
	if(!m_app_should_close)
		Core::LogMessageWarning("Brutal shutdown ! Maybe some layers didn't unload correctly !");
	if(current_application == this)
		current_application = nullptr;
}

void Core::Application::Init()
{
	AssetManager::Init();
	Time::Init();

	LevelManager::Init();
	
	m_physics_engine->Init();

	m_transform_system = new TransformSystem();
	m_transform_system->Register();
	m_transform_system->Connect();

	m_camera_system = new CameraSystem();
	m_camera_system->Connect();
	m_camera_system->Register();

	m_rigidbody_system = new RigidBodySystem();
	m_rigidbody_system->Connect();
	m_rigidbody_system->ConnectPhysicsEvents();
	m_rigidbody_system->Register();

	m_meshcollider_system = new MeshColliderSystem();
	m_meshcollider_system->Register();

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

		accumulator += Time::delta_time;

		Input::UpdateMousePosition();
		Input::UpdateMouseDelta();

		PollEvents();
		m_window->SwapBuffers();

		dt = Time::delta_time;

		for (Layer* _layer : m_layer_stack)
			_layer->OnUpdate(dt);

		LevelManager::OnUpdate(dt);

		if(accumulator > MAX_ACCUMULATED)
		{
			accumulator = MAX_ACCUMULATED;
		}

		while(accumulator >= FIXED_DT)
		{
			for(Layer* _layer : m_layer_stack)
				_layer->OnPhysicsUpdate();
			m_physics_engine->Update(FIXED_DT);
			accumulator -= FIXED_DT;
		}

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

	m_physics_engine->Shutdown();
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

	delete m_transform_system;
	delete m_rigidbody_system;
	delete m_meshcollider_system;
	delete m_camera_system;
}
