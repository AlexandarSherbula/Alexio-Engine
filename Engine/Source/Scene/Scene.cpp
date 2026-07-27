#include "aio_pch.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "Components.hpp"
#include "Renderer/Renderer.hpp"

namespace aio
{
	Scene::Scene()
	{
	}
	
	Scene::~Scene()
	{
	}
	
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { mRegistry.create(), this };
		entity.AddComponent<TransformComponent>();
		TagComponent& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Undefined" : name;
		
		return entity;
	}
	
	void Scene::OnUpdate()
	{
		auto group = mRegistry.group<TransformComponent>(entt::get<SpriteComponent>);
		for (auto entity : group)
		{
			auto& transform = group.get<TransformComponent>(entity);
			auto& sprite = group.get<SpriteComponent>(entity);
		
			glm::mat4 matrix = transform.GetTransform();
			Renderer::DrawQuad(matrix, sprite.Color);
		}
	}
	
	void Scene::OnDestroy()
	{
	}
}
