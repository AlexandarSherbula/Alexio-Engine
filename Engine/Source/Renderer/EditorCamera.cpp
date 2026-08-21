#include "aio_pch.hpp"

#include "EditorCamera.hpp"
#include "Input/Input.hpp"
#include "Utils/Timer.hpp"

#include "Renderer.hpp"

namespace aio
{
	EditorCamera::EditorCamera(float aspectRatio)
	{
		mAspectRatio = aspectRatio;
		mZoomLevel = 10.0f;

		mProjection = glm::ortho(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel, -1.0f, 1.0f);
		mView = glm::mat4x4(1.0f);
		mViewProjection = mProjection * mView;

		mPosition = { 0.0f, 0.0f };
		mRotation = 0.0f;
	}

	void EditorCamera::OnMove()
	{
		float moveSpeed = mZoomLevel;

		if (Input::GetKeyboard()->IsHeld(A))
			mPosition.x += moveSpeed * AppTimer::DeltaTime();
		if (Input::GetKeyboard()->IsHeld(D))
			mPosition.x -= moveSpeed * AppTimer::DeltaTime();
		if (Input::GetKeyboard()->IsHeld(W))
			mPosition.y += moveSpeed * AppTimer::DeltaTime();
		if (Input::GetKeyboard()->IsHeld(S))
			mPosition.y -= moveSpeed * AppTimer::DeltaTime();
	}

	void EditorCamera::OnUpdate()
	{
		mView = glm::inverse((glm::translate(glm::mat4x4(1.0f), glm::vec3(mPosition, 0.0f)) *
			glm::rotate(glm::mat4x4(1.0f), glm::radians(mRotation), glm::vec3(0, 0, 1))));
		mViewProjection = mProjection * mView;

		Renderer::GetProjectionBuffer()->SetData(&mViewProjection, sizeof(glm::mat4x4));
		Renderer::GetProjectionBuffer()->Bind(0);
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(AIO_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		mZoomLevel -= e.GetYOffset() * 0.25f;
		mZoomLevel = std::max(mZoomLevel, 0.25f);
		mProjection = glm::ortho(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel);
		return false;
	}
}