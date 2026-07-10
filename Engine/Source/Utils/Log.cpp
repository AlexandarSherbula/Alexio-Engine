#include "aio_pch.hpp"
#include "Log.hpp"

namespace aio
{
	std::shared_ptr<spdlog::logger> Log::sLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %l: %v%$");
		sLogger = spdlog::stdout_color_mt("Alexio");
		sLogger->set_level(spdlog::level::trace);
	}
	
}

	

