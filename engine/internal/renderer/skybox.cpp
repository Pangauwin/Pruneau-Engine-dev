#include "renderer/skybox.h"
#include "renderer/shader.h"
#include "core/log/log.h"


#include <glad/glad.h>
#include <stb_image.h>

static const char* skybox_vert = R"(
#version 460 core
out vec2 vNDC;
void main()
{
    // fullscreen triangle, no VBO needed
    vec2 pos = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    vNDC = pos * 2.0 - 1.0;
    gl_Position = vec4(vNDC, 1.0, 1.0); // pinned to far plane
}
)";

static const char* skybox_frag = R"(
#version 460 core
in vec2 vNDC;
out vec4 FragColor;

uniform sampler2D equirectMap;
uniform mat4 inv_proj;
uniform mat4 inv_view_rot;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 dir)
{
    vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec4 clip = vec4(vNDC, 1.0, 1.0);
    vec4 view_space = inv_proj * clip;
    view_space /= view_space.w;

    vec3 dir = normalize((inv_view_rot * vec4(view_space.xyz, 0.0)).xyz);
    vec3 hdr = texture(equirectMap, SampleSphericalMap(dir)).rgb;

    // your framebuffer color attachment is 8-bit RGBA, so tonemap+gamma here
    hdr = hdr / (hdr + vec3(1.0));
    hdr = pow(hdr, vec3(1.0 / 2.2));

    FragColor = vec4(hdr, 1.0);
}
)";

Renderer::Skybox::Skybox(const char* _hdr_path)
{
	stbi_set_flip_vertically_on_load(true);

	int w, h, channels;
	float* data = stbi_loadf(_hdr_path, &w, &h, &channels, 3);

	if (!data)
	{
		Core::LogMessageError("Failed to load HDRI: " + std::string(_hdr_path));
		return;
	}

	glGenTextures(1, &m_hdr_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_hdr_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	m_shader = std::make_unique<Shader>(skybox_vert, skybox_frag);
	glGenVertexArrays(1, &m_dummy_vao); // no attributes, gl_VertexID drives it
}

Renderer::Skybox::~Skybox()
{
	if (m_hdr_texture_id) glDeleteTextures(1, &m_hdr_texture_id);
	if (m_dummy_vao) glDeleteVertexArrays(1, &m_dummy_vao);
}

void Renderer::Skybox::Draw(const glm::mat4& _view, const glm::mat4& _projection)
{
	if (!IsValid() || !m_shader) return;

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	m_shader->Bind();

	glm::mat4 view_rot = glm::mat4(glm::mat3(_view)); // strip translation
	m_shader->SetMat4("inv_proj", glm::inverse(_projection));
	m_shader->SetMat4("inv_view_rot", glm::inverse(view_rot));
	m_shader->SetInt("equirectMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdr_texture_id);

	glBindVertexArray(m_dummy_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}