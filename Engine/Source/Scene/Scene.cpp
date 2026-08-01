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
		Mat4x4* mainCameraProjection = nullptr;
		Mat4x4* mainCameraTransform = nullptr;

		{
			auto view = mRegistry.view<TransformComponent, CameraComponent>();
			for (auto [entity, transform, camera] : view.each())
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
			
				if (camera.Primary)
				{
					mainCameraProjection = &camera.camera.GetProjection();
					mainCameraTransform = &transform.GetTransform();
					break;
				}
			}
		}

		if (mainCameraProjection)
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
	}
	
	void Scene::OnDestroy()
	{
	}

	void Scene::OnViewportResize(float width, float height)
	{
		mViewportSize.x = width;
		mViewportSize.y = height;

		auto view = mRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			cameraComponent.camera.SetViewportSize(width, height);
		}
	}
}
