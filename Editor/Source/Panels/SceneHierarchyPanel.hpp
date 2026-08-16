#pragma once

#include "Utils/Utilities.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"

namespace aio
{



	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	public:
		bool EntityDeleted = false;
		bool RenamingEntity = false;
	private:
		Entity mSelectedEntity;
		Ref<Scene> mContext;
		char mRenameBuffer[256];
	};
}