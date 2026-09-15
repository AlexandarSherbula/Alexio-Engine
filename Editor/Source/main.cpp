#include "Editor.hpp"
#include <array>

int main()
{
	aio::AppConfiguration appCfg;
	appCfg.windowCfg.title  = "Alexio Engine";
	appCfg.windowCfg.width  = 1600;
	appCfg.windowCfg.height = 900;
	appCfg.windowCfg.isFullScreen = false;
	appCfg.windowCfg.startMaximized = true;
	appCfg.graphicsAPI = GraphicsAPI::DX11;

	AIO_BEGIN_SESSION("Start", "Start.json");
	Editor editor(appCfg);
	AIO_END_SESSION();

	AIO_BEGIN_SESSION("Runtime", "Runtime.json");
	editor.Run();
	AIO_END_SESSION();
	return 0;
}