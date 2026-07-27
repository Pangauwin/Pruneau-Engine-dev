#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace Renderer
{

class Shader;

class Skybox
{
public:
    Skybox(const char* _hdr_path); /* TODO: refactor in order to implement it with
        the assetmanager*/
    ~Skybox();

    void Draw(const glm::mat4& _view, const glm::mat4& _projection);

    bool IsValid() const { return m_hdr_texture_id != 0; };

private:
    unsigned int m_hdr_texture_id = 0;
    unsigned int m_dummy_vao = 0;

    std::shared_ptr<Shader> m_shader;
};

}