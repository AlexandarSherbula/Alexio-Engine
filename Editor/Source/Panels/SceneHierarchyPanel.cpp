#include "SceneHierarchyPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace aio
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		mContext = context;
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		mContext = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
        ImGui::Begin("Scene Hierarchy");
        {
            auto& registry = mContext->Reg();
            auto view = registry.view<TagComponent>();

            for (auto entityHandle : view)
            {
                Entity entity{ entityHandle, mContext.get() };
                
                DrawEntityNode(entity);
            }
        }
        ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
        TagComponent& tagComponent = entity.GetComponent<TagComponent>();
        ImGuiTreeNodeFlags flags =
            ((mSelectionEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0)
            | ImGuiTreeNodeFlags_OpenOnArrow
            | ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx(
            (void*)(uint64_t)(uint32_t)entity,
            flags,
            tagComponent.Tag.c_str()
        );

        if (ImGui::IsItemClicked())
            mSelectionEntity = entity;

        if (opened)
            ImGui::TreePop();
	}
}