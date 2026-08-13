#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>

#include "Log.hpp"

#if defined(AIO_DEBUG)
	#define ASSETS_DIRECTORY GetProjectDirectory() / "Editor" / "Assets"
#else
	#define ASSETS_DIRECTORY std::filesystem::path("Assets")
#endif


#define CACHE_DIRECTORY ASSETS_DIRECTORY / "cache"

namespace aio
{
	std::filesystem::path GetProjectDirectory();

	std::string ReadFromFiles(const std::filesystem::path& filepath);

	std::string GetFileName(const std::filesystem::path& filepath);

	struct FileDialog
	{
		static std::filesystem::path Open(const char* filter, const std::filesystem::path& defaultPath = ASSETS_DIRECTORY);
		static std::filesystem::path Save(const char* filter, const std::filesystem::path& defaultPath = ASSETS_DIRECTORY);
	};
}

