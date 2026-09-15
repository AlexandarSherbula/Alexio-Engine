#include "Sandbox.hpp"
#include <array>

int main()
{
	aio::AppConfiguration appCfg;
	appCfg.windowCfg.title  = "Alexio Engine";
	appCfg.windowCfg.width  = 1280;
	appCfg.windowCfg.height = 720;
	appCfg.windowCfg.isFullScreen = false;
	appCfg.graphicsAPI = GraphicsAPI::DX11;

	AIO_BEGIN_SESSION("Start", "Start.json");
	Sandbox sandbox(appCfg);
	AIO_END_SESSION();

	AIO_BEGIN_SESSION("Runtime", "Runtime.json");
	sandbox.Run();
	AIO_END_SESSION();
	return 0;
}