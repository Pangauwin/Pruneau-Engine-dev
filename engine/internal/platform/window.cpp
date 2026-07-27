#include "platform/window.h"

#include "components/camera.h"
#include "level/level.h"
#include "platform/platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#ifdef PLATFORM_WINDOWS
#include <Windows.h> // Message boxes
#endif

#include "core/application.h"

#include "input/input.h"

#include "core/events/file_drop.h"
#include "core/events/mouse_button.h"
#include "core/events/keyboard.h"
#include "core/events/char.h"
#include "core/events/cursor_position.h"

#include "level/level_manager.h"
#include <entt/entt.hpp>

#pragma region glfw_callbacks

static void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	Core::Application::Get()->m_window->params.height = height;
	Core::Application::Get()->m_window->params.width = width;

	if(Core::Level* level = Core::LevelManager::GetCurrentLevel())
	{
		for(auto& cam: level->GetRegistry().view<Core::Camera>())
		{
			auto& cam_comp = level->GetRegistry().get<Core::Camera>(cam);
			if(cam_comp.config.type == Core::CAMERA_TYPE_PERSPECTIVE)
			{
				static_cast<Core::CameraPerspectiveData*>(cam_comp.config.data)->aspect = (float)width / (float)height;
			}
			else {
				static_cast<Core::CameraOrthographicData*>(cam_comp.config.data)->left = -(float)width / 2;
				static_cast<Core::CameraOrthographicData*>(cam_comp.config.data)->right = (float)width / 2;
				static_cast<Core::CameraOrthographicData*>(cam_comp.config.data)->top = -(float)height / 2;
				static_cast<Core::CameraOrthographicData*>(cam_comp.config.data)->bottom = (float)height / 2;
			}
			cam_comp.dirty = true;
		}
	}
}

static void window_close_callback(GLFWwindow* window) {
	Core::Application::Get()->CloseApplication();
}

static void drop_file_callback(GLFWwindow* window, int count, const char** paths)
{
	std::vector<std::string> filepaths;
	filepaths.reserve(count);

	for (int i = 0; i < count; i++)
	{
		filepaths.emplace_back(paths[i]);
	}

	Core::FileDropEvent event(std::move(filepaths));

	Core::Application::Get()->OnEvent(event);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{	
	Core::Input::ChangeMouseState((Core::MOUSE_BUTTON)button, action == GLFW_RELEASE ? Core::MOUSE_BUTTON_STATE_RELEASED : Core::MOUSE_BUTTON_STATE_PRESSED);

	Core::MouseButtonEvent event(button, action, mods);
	Core::Application::Get()->OnEvent(event);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Core::Input::ChangeKeyState(key, action == GLFW_RELEASE ? Core::KEY_STATE_RELEASED : Core::KEY_STATE_PRESSED);

	Core::KeyboardEvent event(key, scancode, action, mods);
	Core::Application::Get()->OnEvent(event);
}

static void char_callback(GLFWwindow* window, unsigned int codepoint)
{
	Core::CharEvent event(codepoint);
	Core::Application::Get()->OnEvent(event);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Core::CursorPosition event(xpos, ypos);
	Core::Application::Get()->OnEvent(event);

	Core::Input::UpdateMousePosition();
}

#pragma endregion


Platform::Window::Window(WindowParams& _params) : params(_params), m_glfw_window(NULL)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_glfw_window = glfwCreateWindow(params.width, params.height, params.title, NULL, NULL);

	if (m_glfw_window == NULL)
	{
		#ifdef PLATFORM_WINDOWS
		MessageBox(NULL, "Window Creation Failed !", NULL, MB_OK);
		#endif

		std::cout << "[FATAL] : Window Creation Failed !" << std::endl;

		glfwTerminate();
	}

	glfwMakeContextCurrent(m_glfw_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		#ifdef PLATFORM_WINDOWS
		MessageBox(NULL, "Glad Loading Procedure Failed !", NULL, MB_OK);
		#endif

		std::cout << "[FATAL] : Glad Loading Procedure Failed !" << std::endl;
		glfwTerminate();
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback([](GLenum src, GLenum type, GLuint id,
		GLenum severity, GLsizei, const GLchar* msg, const void*) {
		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
			fprintf(stderr, "[GL] %s\n", msg);
	}, nullptr);

	glViewport(0, 0, params.width, params.height);

	glfwSetFramebufferSizeCallback(m_glfw_window, frame_buffer_size_callback);
	glfwSetWindowCloseCallback(m_glfw_window, window_close_callback);
	glfwSetDropCallback(m_glfw_window, drop_file_callback);

	glfwSetMouseButtonCallback(m_glfw_window, mouse_button_callback);
	glfwSetCursorPosCallback(m_glfw_window, cursor_position_callback);
	glfwSetKeyCallback(m_glfw_window, key_callback);
	glfwSetCharCallback(m_glfw_window, char_callback);

	glfwSwapInterval(0);
}

Platform::Window::~Window()
{
	window_close_callback(m_glfw_window);
	glfwTerminate();
}

void Platform::Window::SwapBuffers()
{
	glfwSwapBuffers(m_glfw_window);
}

void Platform::Window::PollEvents()
{
	glfwPollEvents();
}

float Platform::Window::GetTime()
{
	return static_cast<float>(glfwGetTime());
}

void Platform::Window::DisableMouse()
{
	glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Platform::Window::EnableMouse()
{
	glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}