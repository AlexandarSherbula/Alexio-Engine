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
            bool itemHovered = false;

            auto& registry = mContext->Reg();
            auto view = registry.view<TagComponent>();

            for (auto entityHandle : view)
            {
                Entity entity{ entityHandle, mContext.get() };
                
                DrawEntityNode(entity);

                if (ImGui::IsItemHovered())
                    itemHovered = true;
            }


            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow) &&
                !ImGui::IsAnyItemHovered())
                mSelectionContext = {};

            if (ImGui::IsWindowHovered() &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
                !itemHovered)
            {
                ImGui::OpenPopup("CreateEmptyEntityPopup");
            }

            if (ImGui::BeginPopup("CreateEmptyEntityPopup"))
            {
                if (ImGui::MenuItem("Create Empty Entity"))
                    mContext->CreateEntity("Empty");

                ImGui::EndPopup();
            }
        }
        ImGui::End();

        ImGui::Begin("Inspector");
        {
            if (mSelectionContext)
            {
                DrawComponents(mSelectionContext);
            }
        }
        ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
        TagComponent& tagComponent = entity.GetComponent<TagComponent>();
        ImGuiTreeNodeFlags flags =
            ((mSelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)
            | ImGuiTreeNodeFlags_OpenOnArrow
            | ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx(
            (void*)(uint64_t)(uint32_t)entity,
            flags,
            tagComponent.Tag.c_str()
        );

        if (ImGui::IsItemClicked())
            mSelectionContext = entity;

        bool isEntityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
                isEntityDeleted = true;

            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
            ImGui::TreeNodeEx((void*)9816582, flags, tagComponent.Tag.c_str());
            ImGui::TreePop();
        }

        if (isEntityDeleted)
        {
            mContext->DestroyEntity(entity);
            if (mSelectionContext == entity)
                mSelectionContext = {};
        }
	}

    template <typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[1];
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGuiTreeNodeFlags treeNodeFlags =
            ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_AllowItemOverlap;
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::PushFont(boldFont);
        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
        ImGui::PopFont();
        ImGui::PopStyleVar();

        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
        {
            ImGui::OpenPopup("Component Settings");
        }

        bool removeComponent = false;
        if (name != "Transform")
        {
            if (ImGui::BeginPopup("Component Settings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                    removeComponent = true;

                ImGui::EndPopup();
            }
        }

        if (open)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
            auto& component = entity.GetComponent<T>();
            uiFunction(component);
            ImGui::PopStyleVar();

            ImGui::TreePop();
            ImGui::Spacing();
            ImGui::Separator();
        }


        if (removeComponent)
            entity.RemoveComponent<T>();
    }

    template <typename T>
    static void AddComponentToPanel(const char* name, Entity selectionEntity)
    {
        if (!selectionEntity.HasComponent<T>())
        {
            if (ImGui::MenuItem(name))
            {
                selectionEntity.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValues = 0.0f, float columnWidth = 100.0f)
    {
        ImGui::PushID(label.c_str());
        
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        
        ImGui::NextColumn();
        
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
        
        float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
        
        if (ImGui::Button("X", buttonSize))
            values.x = resetValues;
        
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValues;
        
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValues;
        
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f);
        ImGui::PopItemWidth();
        
        ImGui::PopStyleVar();
        
        ImGui::Columns(1);
        
        ImGui::PopID();
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            TagComponent& tagComponent = entity.GetComponent<TagComponent>();

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy(buffer, tagComponent.Tag.c_str(), sizeof(buffer));
            if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
            {
                tagComponent.Tag = std::string(buffer);
            }
            ImGui::Separator();
        }

        if (entity.HasComponent<TransformComponent>())
        {
            DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
            {
                DrawVec3Control("Position", component.Position);
                Vector3 rotationInDegrees = glm::degrees(component.Rotation);
                DrawVec3Control("Rotation", rotationInDegrees);
                component.Rotation = glm::radians(rotationInDegrees);
                DrawVec3Control("Scale", component.Scale, 1.0f);
            });
        }

        if (entity.HasComponent<CameraComponent>())
        {
            DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
            {
                if (ImGui::BeginTable("CameraComponentTable", 2, ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("Primary");

                    ImGui::TableNextColumn();

                    // Center the checkbox inside the column
                    float columnWidth = ImGui::GetColumnWidth(1); // column index 1 = second column
                    float checkboxWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2.0f;

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - checkboxWidth) * 0.5f);
                    ImGui::Checkbox("##PrimaryCamera", &component.Primary);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Size");

                    ImGui::TableNextColumn();

                    ImGui::SetNextItemWidth(-FLT_MIN);
                    if (ImGui::DragFloat("##CameraSize", &component.OrthographicSize, 0.1f, 0.0f, 0.0f, "%.3f"))
                        component.Camera.SetOrthographicSize(component.OrthographicSize);

                    ImGui::EndTable();
                }
            });
        }

        if (entity.HasComponent<SpriteComponent>())
        {
            DrawComponent<SpriteComponent>("Sprite", entity, [](auto& component)
            {
                ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
            });
        }

        // Add spacing before the button
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        // --- Centered Add Component Button ---
        float regionWidth = ImGui::GetContentRegionAvail().x;
        float regionHeight = ImGui::GetContentRegionAvail().y;
        float buttonWidth = 200.0f; // choose any width you like
        float buttonHeight = 30.0f; // choose any width you like

        float cursorX = (regionWidth - buttonWidth) * 0.5f;
        ImGui::SetCursorPosX(cursorX);

        if (ImGui::Button("Add Component", ImVec2{ buttonWidth, buttonHeight }))
            ImGui::OpenPopup("AddComponentPopup");

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            AddComponentToPanel<CameraComponent>("CameraComponent", mSelectionContext);
            AddComponentToPanel<SpriteComponent>("SpriteComponent", mSelectionContext);

            ImGui::EndPopup();
        }
    }
}