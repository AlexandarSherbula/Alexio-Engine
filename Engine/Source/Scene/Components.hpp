#pragma once

#include "../Utils/Math.hpp"
#include "SceneCamera.hpp"

namespace aio
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {
		}
	};

	struct TransformComponent
	{
		Vector3 Position = { 0.0f, 0.0f, 0.0f };
		Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
		Vector3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const Vector3& position)
			: Position(position) {
		}

		Mat4x4& GetTransform()
		{
			Mat4x4 rotation = glm::rotate(glm::mat4x4(1.0f), Rotation.x, { 1.0f, 0.0f, 0.0f })
				* glm::rotate(glm::mat4x4(1.0f), Rotation.y, { 0.0f, 1.0f, 0.0f })
				* glm::rotate(glm::mat4x4(1.0f), Rotation.z, { 0.0f, 0.0f, 1.0f });

			Mat4x4 translate = glm::translate(glm::mat4x4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4x4(1.0f), Scale);

			return translate;
		}
	};

	struct SpriteComponent
	{
		Vector4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const Vector4& color)
			: Color(color) {
		}
	};


	struct CameraComponent
	{
		SceneCamera camera;
		bool Primary = true;
		float OrthographicSize = 10.0f;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}