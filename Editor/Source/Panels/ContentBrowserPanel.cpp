#include "ContentBrowserPanel.hpp"
#include "Utils/FileUtils.hpp"

#include <imgui.h>
#include <imgui_internal.h>

/// <summary>
/// Folder icon used here is by kmg design. "open-folder-B&W.png" is a modification of this folder icon
/// <a href="https://www.flaticon.com/free-icon/open-folder_5082720?term=open+folder&page=1&position=4&origin=tag&related_id=5082720" title="open folder icons">Open folder icons created by kmg design - Flaticon</a>
/// File icon: 
/// <a href = "https://www.flaticon.com/free-icon/file_2521594?term=file&related_id=2521594" title = "paper icons">Paper icons created by iconixar - Flaticon< / a>
/// </summary>

namespace aio
{
	TextureConfiguration textureCFG;
	ContentBrowserPanel::ContentBrowserPanel()
	{
		textureCFG.SamplerWrap = TextureWrap::Clamp;
		mCurrentDirectory = ASSETS_DIRECTORY;

		mFolderIcon = Texture::Create(textureCFG, ASSETS_DIRECTORY / "images" / "open-folder-B&W.png");
		mFileIcon = Texture::Create(textureCFG, ASSETS_DIRECTORY / "images" / "file.png");

		mRenamingFiles = false;

		mTimeClicked = 0.0f;
		mLastTimeClicked = 0.0f;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
        double now = ImGui::GetTime();

        ImGui::Begin("Content Browser");

        if (mCurrentDirectory != ASSETS_DIRECTORY)
        {
            if (ImGui::Button("Back"))
                mCurrentDirectory = mCurrentDirectory.parent_path();
        }

        const float padding = 16.0f;
        const float thumbnailSize = 128.0f;
        const float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int32_t columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& entry : std::filesystem::directory_iterator(mCurrentDirectory))
        {
            auto& path = entry.path();
            std::string filenameString = path.filename().string();

            ImGui::PushID(filenameString.c_str());

            // --- Thumbnail selection ---
            std::string fileExtension = path.extension().string();
            bool isImage = fileExtension == ".png" || fileExtension == ".jpg" || fileExtension == ".jpeg";

            Ref<Texture> icon;
            if (isImage)
            {
                if (mThumbnailCache.contains(path.string()))
                    icon = mThumbnailCache[path.string()];
                else
                {
                    icon = Texture::Create(textureCFG, path.string());
                    mThumbnailCache[path.string()] = icon;
                }
            }
            else
            {
                icon = entry.is_directory() ? mFolderIcon : mFileIcon;
            }

            // --- Draw group (icon + text) ---
            ImGui::BeginGroup();
            ImVec2 itemStart = ImGui::GetCursorScreenPos();

            ImGui::Image(reinterpret_cast<ImTextureID>(icon->GetHandle()), { thumbnailSize, thumbnailSize });

            ImVec2 itemEnd = { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y };
            ImGui::EndGroup();

            // --- Invisible button over whole area ---
            ImGui::SetCursorScreenPos(itemStart);
            ImVec2 buttonSize = { itemEnd.x - itemStart.x, itemEnd.y - itemStart.y };
            ImGui::InvisibleButton("##select", buttonSize);

            // --- Click handling ---
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                if (mSelectedPath == path)
                {
                    double delta = now - mLastTimeClicked;

                    if (delta < 0.3) // fast double-click → open
                    {
                        if (entry.is_directory())
                            mCurrentDirectory /= path.filename();
                        //else
                        //    OpenFile(path);
                        mRenamingFiles = false;
                    }
                    else if (delta > 0.5) // slow double-click → rename
                    {
                        mRenamingFiles = true;
                        strcpy(mRenameBuffer, filenameString.c_str());
                    }
                }
                else
                {
                    // new selection
                    mSelectedPath = path;
                    mRenamingFiles = false;
                }

                mLastTimeClicked = now;
            }

            if (mSelectedPath == path)
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRect(itemStart, itemEnd, IM_COL32(66, 150, 250, 255), 4.0f, 0, 2.0f);
            }

            if (mRenamingFiles && mSelectedPath == path)
            {
                // Draw InputText exactly where the filename would be
                ImGui::SetCursorPosY(ImGui::GetCursorPosY()); // keep same vertical position
                ImGui::InputText("##Rename", mRenameBuffer, sizeof(mRenameBuffer),
                    ImGuiInputTextFlags_EnterReturnsTrue);

                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    std::filesystem::path newPath = path.parent_path() / mRenameBuffer;
                    std::filesystem::rename(path, newPath);
                    mRenamingFiles = false;
                }
            }
            else
            {
                // Normal filename text
                ImGui::TextWrapped(filenameString.c_str());
            }

            ImGui::NextColumn();
            ImGui::PopID();
        }

        // --- Deselect when clicking empty space ---
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow) &&
            !ImGui::IsAnyItemHovered())
        {
            mSelectedPath.clear();
            mRenamingFiles = false;
        }

        ImGui::Columns(1);
        ImGui::End();
    }
}