#include "renderer/renderer.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "renderer/frame_buffer.h"

#include <algorithm>

#include "core/entity.h"

#include "components/model_renderer.h"

#include "core/application.h"

static Renderer::Renderer* _current_renderer = nullptr;

Renderer::Shader* Renderer::default_shader = nullptr;

Renderer::Renderer::Renderer(Platform::Window* _window) : 
	m_window(_window), m_frame_buffer(Framebuffer(0, 0)), m_camera_index(0)
{
	_current_renderer = this;
}

Renderer::Renderer::~Renderer()
{
}

Renderer::Renderer* Renderer::Renderer::Get()
{
	return _current_renderer;
}

void Renderer::Renderer::RegisterModel(Core::ModelRenderer* _model)
{
	m_render_pool.push_back(_model);
}

void Renderer::Renderer::UnRegisterModel(Core::ModelRenderer* _model)
{
	m_render_pool.erase(find(m_render_pool.begin(), m_render_pool.end(), _model));
}

void Renderer::Renderer::RegisterCamera(Core::Camera* _camera)
{
	m_cameras.push_back(_camera);
}

void Renderer::Renderer::UnRegisterCamera(Core::Camera* _camera)
{
	m_cameras.erase(find(m_cameras.begin(), m_cameras.end(), _camera));
}

void Renderer::Renderer::PreRender()
{
	m_frame_buffer.Bind();

	glClearColor(0.00f, 0.00f, 0.70f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Renderer::PostRender()
{
	if (m_cameras.size() != 0) {

		if (m_camera_index >= m_cameras.size())
		{
			Core::LogMessageInfo("Camera Index wasn't valid, switching camera index to 0");

			m_camera_index = 0;
		}

		else
		{
			Core::Camera* _current_camera = m_cameras[m_camera_index];

			//TODO : switch between perspective and orthographic view
			glm::mat4 perspective = *_current_camera->GetPerspective();
			glm::mat4 world_transform = _current_camera->GetOwner()->GetComponent<Core::Transform>()->GetWorldTransformMatrix();
			glm::mat4 view = glm::inverse(world_transform);

			for (Core::ModelRenderer* _model_renderer : m_render_pool)
			{
				if (!_model_renderer->model) continue;
				glm::mat4 _model = _model_renderer->GetOwner()->GetComponent<Core::Transform>()->GetWorldTransformMatrix();


				_model_renderer->model->Draw(view, _model, perspective);
			}
		}
	}
	m_frame_buffer.UnBind();
}

void Renderer::Renderer::PreGUIRender()
{
	
}

void Renderer::Renderer::PostGUIRender()
{
	
}
