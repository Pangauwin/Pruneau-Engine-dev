#include "renderer/renderer.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "components/camera.h"
#include "components/model_renderer.h"
#include "components/transform.h"
#include "core/application.h"
#include "entt/entity/fwd.hpp"
#include "level/level.h"
#include "renderer/frame_buffer.h"

#include "level/level_manager.h"

#include "asset/asset_manager.h"
#include "asset/asset.h"

#include "renderer/skybox.h"

#include <entt/entt.hpp>
#include <string>

static Renderer::Renderer* _current_renderer = nullptr;

Renderer::Shader* Renderer::default_shader = nullptr;

static float quad_vertices[] = {
    // pos              // uv
    -1.f, -1.f, 0.f, 0.f,
     1.f, -1.f, 1.f, 0.f,
     1.f,  1.f, 1.f, 1.f,

    -1.f, -1.f, 0.f, 0.f,
     1.f,  1.f, 1.f, 1.f,
    -1.f,  1.f, 0.f, 1.f,
};

static GLuint screen_vao = 0;
static GLuint screen_vbo = 0;
static GLuint screen_program = 0;

Renderer::Renderer::Renderer(Platform::Window* _window) : 
	m_window(_window), m_frame_buffer(Framebuffer(_window->params.width, _window->params.height))
{
	_current_renderer = this;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &screen_vao);
    glGenBuffers(1, &screen_vbo);

    glBindVertexArray(screen_vao);

    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    const char* vs = R"(
        #version 460 core

        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;

        out vec2 TexCoord;

        void main()
        {
            TexCoord = aUV;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fs = R"(
        #version 460 core

        in vec2 TexCoord;

        uniform sampler2D screenTexture;

        out vec4 FragColor;

        void main()
        {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vs, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fs, nullptr);
    glCompileShader(frag);

    screen_program = glCreateProgram();
    glAttachShader(screen_program, vert);
    glAttachShader(screen_program, frag);
    glLinkProgram(screen_program);

    glDeleteShader(vert);
    glDeleteShader(frag);

}

Renderer::Renderer::~Renderer()
{
}

Renderer::Renderer* Renderer::Renderer::Get()
{
	return _current_renderer;
}

void Renderer::Renderer::PreRender()
{
	m_frame_buffer.Bind();

	glClearColor(0.00f, 0.00f, 0.70f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Renderer::PostRender()
{
	entt::registry& _registry = Core::LevelManager::GetCurrentLevel()->GetRegistry();

	bool camera_found = false;
	
	for(auto& _camera: _registry.view<Core::Camera, Core::Transform>())
	{
		Core::Camera& cam = _registry.get<Core::Camera>(_camera);
		if(cam.index == Core::LevelManager::GetCurrentLevel()->camera_index)
		{
			camera_found = true;

			glm::mat4 projection = cam.projection;
			glm::mat4 world_transform = _registry.get<Core::Transform>(_camera).world_transform;
			glm::mat4 view = glm::inverse(world_transform);

            if(m_skybox)
                m_skybox->Draw(view, projection);

			for(auto& entity: _registry.view<Core::ModelRenderer, Core::Transform>())
			{
				if(auto model_asset = 
					Core::AssetManager::GetAsset<Core::ModelAsset>(_registry.get<Core::ModelRenderer>(entity).model_id))
				{
					glm::mat4 _model = _registry.get<Core::Transform>(entity).world_transform;

                    model_asset->Draw(projection, view, _model);
				}
			}
		}
		else if(cam.index == 0)
		{
			cam.index = Core::CameraSystem::GetNextCameraIndex();
		}
	}

	if(!camera_found)
	{
		Core::LogMessageError("No camera corresponding to camera index found !");
	}

	m_frame_buffer.UnBind();

    //TODO: Write an actual target for the renderer to know where to render (texture ? screen ?)
}

void Renderer::Renderer::PreGUIRender()
{
	
}

void Renderer::Renderer::PostGUIRender()
{
	
}
