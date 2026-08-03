#pragma once
#include "Alexio.hpp"

namespace aio
{
	class CameraController : public EntityScript
	{
	public:
		void OnCreate() override
		{
		}

		void OnUpdate() override
		{
			auto& transform = GetComponent<TransformComponent>();
			float speed = 5.0f;

			if (Input::GetKeyboard()->IsHeld(LEFT))
				transform.Position.x -= speed * AppTimer::DeltaTime();
			if (Input::GetKeyboard()->IsHeld(RIGHT))
				transform.Position.x += speed * AppTimer::DeltaTime();
			if (Input::GetKeyboard()->IsHeld(UP))
				transform.Position.y += speed * AppTimer::DeltaTime();
			if (Input::GetKeyboard()->IsHeld(DOWN))
				transform.Position.y -= speed * AppTimer::DeltaTime();
		}

		void OnDestroy() override
		{
		}
	};
}
