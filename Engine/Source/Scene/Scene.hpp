#pragma once
#include <entt/entt.hpp>
#include "Utils/Math.hpp"

namespace aio
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();
	
		Entity CreateEntity(const std::string& name = std::string());
	
		void OnUpdate();
		void OnDestroy();
	
		inline entt::registry& Reg() { return mRegistry; }

		void OnViewportResize(float width, float height);
	private:
		entt::registry mRegistry;
		Vector2 mViewportSize;

		friend class SceneHierarchyPanel;
	};
}