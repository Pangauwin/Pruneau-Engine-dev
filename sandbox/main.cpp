#include "core/application.h"

#include "layer/sandbox_layer.h"

int main()
{
    Core::AppParams params;
    params.window_params.title = "Sandbox";
    params.window_params.height = 720;
    params.window_params.width = 1080;

    Core::Application app(params);
    app.PushLayer(new SandboxLayer());
    app.Run();
}