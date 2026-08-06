#include "core/application.h"

#include "layer/sandbox_layer.h"
#include "renderer/renderer.h"

int main()
{
    Core::AppParams params;
    params.window_params.title = "Sandbox";
    params.window_params.height = 720;
    params.window_params.width = 1080;

    params.renderer_config._target = Renderer::RenderTarget::RENDER_TARGET_TEXTURE;

    Core::Application app(params);
    app.PushLayer(new Sandbox::SandboxLayer());
    app.Run();
}