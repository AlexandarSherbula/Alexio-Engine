#pragma once

#include "Utils/Utilities.hpp"
#include "Renderer/Texture.hpp"

#include <filesystem>
	
namespace aio
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();
		void OpenFile(const std::filesystem::path& filePath);
	public:
		bool LoadScene = false;
		std::filesystem::path FilePathForLoading;
		std::filesystem::path SelectedPath;
		bool RenamingFiles;
	private:
		std::filesystem::path mCurrentDirectory;
		Ref<Texture> mFolderIcon;
		Ref<Texture> mFileIcon;
		std::unordered_map<std::string, Ref<Texture>> mThumbnailCache;

		double mCurrentTime;
		double mTimeClicked;
		double mLastTimeClicked;

		char mRenameBuffer[256];
	};
}