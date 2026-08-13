#include "aio_pch.hpp"
#include "FileUtils.hpp"

namespace aio
{
	std::filesystem::path GetProjectDirectory()
	{
		return std::filesystem::current_path().parent_path().parent_path();
	}

	std::string ReadFromFiles(const std::filesystem::path& filepath)
	{
		std::string result = "";
		if (std::filesystem::exists(filepath))
		{
			std::ifstream in(filepath, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				result.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
				in.close();
			}
			else
			{
				AIO_LOG_ERROR("Could not open file '{0}'", filepath.string());
			}
		}
		
		return result;
	}

	std::string GetFileName(const std::filesystem::path& filepath)
	{
		AIO_ASSERT(std::filesystem::exists(filepath), "Cannot get name from filepath " + filepath.string());

		return filepath.stem().string();
	}


	std::filesystem::path FileDialog::Open(const char* filter, const std::filesystem::path& defaultPath)
	{
		std::string defaultPath_str = defaultPath.string();
		
		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_OpenDialog(filter, defaultPath_str.c_str(), &outPath);

		std::filesystem::path savedPath;

		if (result == NFD_OKAY)
		{
			savedPath = outPath;
			free(outPath);
		}
		else if (result == NFD_ERROR)
		{
			AIO_LOG_ERROR("NFD Error: {0}", NFD_GetError());
		}

		return savedPath;
	}

	std::filesystem::path FileDialog::Save(const char* filter, const std::filesystem::path& defaultPath)
	{
		std::string defaultPath_str = defaultPath.string();

		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_SaveDialog(filter, defaultPath_str.c_str(), &outPath);

		std::filesystem::path savedPath;

		if (result == NFD_OKAY)
		{
			savedPath = outPath;
			free(outPath);
		}
		else if (result == NFD_ERROR)
		{
			AIO_LOG_ERROR("NFD Error: {0}", NFD_GetError());
		}

		return savedPath;
	}
}


