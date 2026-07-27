#pragma once
#include <entt/entt.hpp>

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
	private:
		entt::registry mRegistry;
	};
}