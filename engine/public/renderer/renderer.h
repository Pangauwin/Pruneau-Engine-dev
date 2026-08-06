#pragma once

#include "platform/window.h"
#include "frame_buffer.h"

#include "shader.h"

namespace Core {
	class Application;
}

namespace Core {
	class ModelRenderer;
	class Camera;
}

namespace Renderer {

class Skybox;

enum class RenderTarget{
	RENDER_TARGET_TEXTURE,
	RENDER_TARGET_SCREEN
};

struct RendererConfig {
	RenderTarget _target = RenderTarget::RENDER_TARGET_SCREEN;
	unsigned int width = 1920;
	unsigned int height = 1080;
};

class Renderer
{
friend class Core::Application;

// Class that can register themselves
friend class Core::ModelRenderer;
friend class Core::Camera;

public:
	Renderer(Platform::Window* _window, RendererConfig _config={});
	~Renderer();

	static Renderer* Get();

private:
	void PreRender();
	void PostRender();
	
	void PreGUIRender();
	void PostGUIRender();

public:
	Framebuffer m_frame_buffer;
	Skybox* m_skybox;

	RendererConfig m_config;

private:
	Platform::Window* m_window;
};

extern Shader* default_shader;
}