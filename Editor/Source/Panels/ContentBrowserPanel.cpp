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
    static double delta = 0.0f;

	ContentBrowserPanel::ContentBrowserPanel()
	{
		mCurrentDirectory = ASSETS_DIRECTORY;

		mFolderIcon = Texture::Create(ASSETS_DIRECTORY / "images" / "open-folder-B&W.png");
		mFileIcon = Texture::Create(ASSETS_DIRECTORY / "images" / "file.png");

		RenamingFiles = false;

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

            bool isImage = path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".jpeg";

            Ref<Texture> icon;
            if (isImage)
            {
                if (mThumbnailCache.contains(path.string()))
                {
                    icon = mThumbnailCache[path.string()];
                }
                else
                {
                    icon = Texture::Create(path.string());
                    mThumbnailCache[path.string()] = icon;
                }
            }
            else
            {
                icon = entry.is_directory() ? mFolderIcon : mFileIcon;
            }


            ImGui::BeginGroup();
            ImVec2 itemStart = ImGui::GetCursorScreenPos();

            ImGui::Image(reinterpret_cast<ImTextureID>(icon->GetHandle()), { thumbnailSize, thumbnailSize });

            ImVec2 itemEnd = { ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y};
            ImGui::EndGroup();

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow) &&
                !ImGui::IsAnyItemHovered())
            {
                SelectedPath.clear();
                if (RenamingFiles)
                    RenamingFiles = false;
                delta = 0.0f;
            }

            ImGui::SetCursorScreenPos(itemStart);
            ImVec2 buttonSize = { itemEnd.x - itemStart.x, itemEnd.y - itemStart.y };
            ImGui::InvisibleButton("##select", buttonSize);

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                if (SelectedPath == path)
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        if (entry.is_directory())
                            mCurrentDirectory /= path.filename();
                        else
                            OpenFile(path);
                        RenamingFiles = false;
                    }
                }
                else
                {
                    SelectedPath = path;
                    RenamingFiles = false;
                }

                mLastTimeClicked = now;
                delta = 0.0f;
            }

            if (SelectedPath == path)
            {
                delta = now - mLastTimeClicked;

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(itemStart, ImVec2(itemEnd.x, itemEnd.y + 25.0f), IM_COL32(66, 150, 250, 127), 4.0f, 0);

                if (RenamingFiles)
                { 
                    ImGui::SetNextItemWidth(thumbnailSize);

                    strcpy(mRenameBuffer, GetFileName(path).c_str());
                    if (ImGui::InputText("##Rename", mRenameBuffer, sizeof(mRenameBuffer),
                        ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        std::filesystem::path newPath = path.parent_path() / mRenameBuffer;
                        std::filesystem::rename(path, newPath);
                        RenamingFiles = false;
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));

                    ImGui::BeginChild("##TextRegion", ImVec2(thumbnailSize, 20), false, ImGuiWindowFlags_NoBackground);

                    ImVec2 textSize = ImGui::CalcTextSize(GetFileName(path).c_str());
                    ImGui::SetCursorPosX((thumbnailSize - textSize.x) * 0.5f);

                    if (ImGui::Selectable(GetFileName(path).c_str(), false,
                        ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        RenamingFiles = true;
                    }

                    ImGui::EndChild();

                    ImGui::PopStyleColor(2);
                }
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));

                ImGui::BeginChild("##TextRegion", ImVec2(thumbnailSize, 20), false, ImGuiWindowFlags_NoBackground);

                ImVec2 textSize = ImGui::CalcTextSize(GetFileName(path).c_str());
                ImGui::SetCursorPosX((thumbnailSize - textSize.x) * 0.5f);

                if (ImGui::Selectable(GetFileName(path).c_str(), false,
                    ImGuiSelectableFlags_AllowDoubleClick))
                {
                    SelectedPath = path;
                }

                ImGui::EndChild();

                ImGui::PopStyleColor(2);
            }

            ImGui::NextColumn();
            ImGui::PopID();
        }

        ImGui::Columns(1);
        ImGui::End();
    }

    void ContentBrowserPanel::OpenFile(const std::filesystem::path& filePath)
    {
        if (filePath.extension() == ".yaml")
        {
            LoadScene = true;
        }
        FilePathForLoading = filePath;
    }
}