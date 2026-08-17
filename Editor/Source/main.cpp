#include "Editor.hpp"
#include <array>

int main()
{
	aio::AppSpecifications appSpecs;
	appSpecs.windowSpecs.title  = "Alexio Engine";
	appSpecs.windowSpecs.width  = 1600;
	appSpecs.windowSpecs.height = 900;
	appSpecs.windowSpecs.isFullScreen = false;
	appSpecs.windowSpecs.startMaximized = true;
	appSpecs.graphicsAPI = GraphicsAPI::DX11;

	AIO_BEGIN_SESSION("Start", "Start.json");
	Editor editor(appSpecs);
	AIO_END_SESSION();

	AIO_BEGIN_SESSION("Runtime", "Runtime.json");
	editor.Run();
	AIO_END_SESSION();
	return 0;
}