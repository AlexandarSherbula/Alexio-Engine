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
	private:
		std::filesystem::path mCurrentDirectory;
		std::filesystem::path mSelectedPath;
		Ref<Texture> mFolderIcon;
		Ref<Texture> mFileIcon;
		std::unordered_map<std::string, Ref<Texture>> mThumbnailCache;

		bool mRenamingFiles;
		double mCurrentTime;
		double mTimeClicked;
		double mLastTimeClicked;

		char mRenameBuffer[256];
	};
}