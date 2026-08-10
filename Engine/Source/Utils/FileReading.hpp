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
	std::string GetFilePath_str(const std::filesystem::path& filepath);
}

