#include "Editor.hpp"
#include <array>

int main()
{
	aio::AppSpecifications appSpecs;
	appSpecs.windowSpecs.title  = "Alexio Engine";
	appSpecs.windowSpecs.width  = 1280;
	appSpecs.windowSpecs.height = 720;
	appSpecs.windowSpecs.isFullScreen = false;
	appSpecs.graphicsAPI = GraphicsAPI::DX11;

	AIO_BEGIN_SESSION("Start", "Start.json");
	Editor Editor(appSpecs);
	AIO_END_SESSION();

	AIO_BEGIN_SESSION("Runtime", "Runtime.json");
	Editor.Run();
	AIO_END_SESSION();
	return 0;
}