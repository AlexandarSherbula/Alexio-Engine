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
		OnDestroy();
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
		{
			auto view = mRegistry.view<NativeScriptComponent>();
			for (auto entity : view) 
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);

				if (!nsc.Instance) {
					nsc.Instance = nsc.InstantiateScript();
					AIO_ASSERT(nsc.Instance, "Instantiate Script returned nullptr!"); // A safety net for if by any off chance nsc. Instance is still nullptr at this point
					nsc.Instance->mEntity = { entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate();
			}
		}


		Mat4x4* mainCameraProjection = nullptr;
		Mat4x4 mainCameraTransform;

		{
			auto view = mRegistry.view<TransformComponent, CameraComponent>();
			for (auto [entity, transform, camera] : view.each())
			{
				transform = view.get<TransformComponent>(entity);
				camera = view.get<CameraComponent>(entity);
			
				if (camera.Primary)
				{
					mainCameraProjection = &camera.Camera.GetProjection();
					mainCameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCameraProjection)
		{
			Mat4x4 viewProj = *mainCameraProjection * glm::inverse(mainCameraTransform);

			Renderer::GetProjectionBuffer()->SetData(&viewProj, sizeof(Mat4x4));
			Renderer::GetProjectionBuffer()->Bind(0);

			auto group = mRegistry.group<TransformComponent>(entt::get<SpriteComponent>);
			for (auto entity : group)
			{
				auto& transform = group.get<TransformComponent>(entity);
				auto& sprite = group.get<SpriteComponent>(entity);

				const glm::mat4& matrix = transform.GetTransform();
				Renderer::DrawQuad(matrix, sprite.Color);
			}

			Renderer::Flush();
		}
	}
	
	void Scene::OnDestroy()
	{
		auto view = mRegistry.view<NativeScriptComponent>();
		for (auto entity : view) {
			auto& nsc = view.get<NativeScriptComponent>(entity);
			if (nsc.Instance) 
			{
				nsc.Instance->OnDestroy();
				nsc.DestroyScript(&nsc);
			}
		}
	}

	void Scene::OnViewportResize(float width, float height)
	{
		mViewportSize.x = width;
		mViewportSize.y = height;

		auto view = mRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			cameraComponent.Camera.SetViewportSize(width, height);
		}
	}
}
