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

	void Scene::DestroyEntity(Entity entity)
	{
		mRegistry.destroy(entity);
	}
	
	void Scene::OnUpdate()
	{
		Mat4x4* mainCameraProjection = nullptr;
		Mat4x4 mainCameraTransform;

		{
			auto view = mRegistry.view<TransformComponent, CameraComponent>();
			for (auto [entityHandle, transform, camera] : view.each())
			{
				transform = view.get<TransformComponent>(entityHandle);
				camera = view.get<CameraComponent>(entityHandle);

				if (camera.Primary)
				{
					mainCameraProjection = &camera.Camera.GetProjection();
					mainCameraTransform = transform.GetTransform();
					break;
				}
			}
		}


		{
			auto view = mRegistry.view<NativeScriptComponent>();
			for (auto entityHandle : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entityHandle);

				if (!nsc.Instance) {
					nsc.Instance = nsc.InstantiateScript();
					AIO_ASSERT(nsc.Instance, "Instantiate Script returned nullptr!"); // A safety net for if by any off chance nsc. Instance is still nullptr at this point
					nsc.Instance->mEntity = { entityHandle, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate();
			}
		}

		if (mainCameraProjection)
		{
			Mat4x4 viewProj = *mainCameraProjection * glm::inverse(mainCameraTransform);
			
			Renderer::GetProjectionBuffer()->SetData(&viewProj, sizeof(Mat4x4));
			Renderer::GetProjectionBuffer()->Bind(0);

			DrawEntities();
		}
	}

	void Scene::OnUpdateEditor(EditorCamera& camera)
	{
		camera.OnUpdate();

		
	}
	
	void Scene::DrawEntities()
	{
		auto group = mRegistry.group<TransformComponent>(entt::get<SpriteComponent>);
		for (auto entityHandle : group)
		{
			auto& transform = group.get<TransformComponent>(entityHandle);
			auto& sprite = group.get<SpriteComponent>(entityHandle);

			const glm::mat4& matrix = transform.GetTransform();
			Renderer::DrawQuad(matrix, sprite.Color, (uint32_t)entityHandle);
		}

		Renderer::Flush();
	}

	void Scene::OnDestroy()
	{
		auto view = mRegistry.view<NativeScriptComponent>();
		for (auto entityHandle : view) {
			auto& nsc = view.get<NativeScriptComponent>(entityHandle);
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
		for (auto entityHandle : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entityHandle);
			cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCamera()
	{
		auto view = mRegistry.view<CameraComponent>();
		for (auto entityHandle : view)
		{
			const auto& cameraEntityHandle = view.get<CameraComponent>(entityHandle);
			if (cameraEntityHandle.Primary)
				return Entity{ entityHandle, this };
		}
		return Entity();
	}
}
