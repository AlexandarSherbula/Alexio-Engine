#pragma once
#include <entt/entt.hpp>
#include "Utils/Math.hpp"
#include "SceneSerializer.hpp"
#include "Renderer/EditorCamera.hpp"

namespace aio
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();
	
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		
		void OnUpdate();
		void OnUpdateEditor(EditorCamera& camera);

		void DrawEntities();
		void OnDestroy();
	
		inline entt::registry& Reg() { return mRegistry; }

		void OnViewportResize(float width, float height);

		Entity GetPrimaryCamera();
	private:
		entt::registry mRegistry;
		Vector2 mViewportSize;

		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};
}