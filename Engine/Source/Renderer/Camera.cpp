#include "aio_pch.hpp"

#include "Camera.hpp"
#include "Input/Input.hpp"
#include "Utils/Timer.hpp"

namespace aio
{
	Ref<ConstantBuffer> Camera::sBuffer = nullptr;

	Camera::Camera(float aspectRatio)
	{
		mAspectRatio = aspectRatio;
		mZoomLevel = 1.0f;

		mProjection = glm::ortho(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, mZoomLevel, -mZoomLevel);
		mView = glm::mat4x4(1.0f);
		mViewProjection = mProjection * mView;

		mPosition = { 0.0f, 0.0f };
		mRotation = 0.0f;

		sBuffer = ConstantBuffer::Create(sizeof(Mat4x4), 0);
	}

	void Camera::OnUpdate(float dt)
	{
		mView = glm::translate(glm::mat4x4(1.0f), glm::vec3(mPosition, 0.0f)) *
			glm::rotate(glm::mat4x4(1.0f), glm::radians(mRotation), glm::vec3(0, 0, 1));

		mViewProjection = mProjection * mView;

		sBuffer->SetData(&mViewProjection, sizeof(glm::mat4x4));
		sBuffer->Bind(0);
	}

	void Camera::UpdateProjection(float aspectRatio)
	{
		mAspectRatio = aspectRatio;
		mProjection = glm::ortho(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, mZoomLevel, -mZoomLevel);
	}

	void Camera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(AIO_BIND_EVENT_FN(Camera::OnWindowResize));
		dispatcher.Dispatch<MouseScrolledEvent>(AIO_BIND_EVENT_FN(Camera::OnMouseScroll));
	}

	bool Camera::OnWindowResize(WindowResizeEvent& e)
	{
		mAspectRatio = static_cast<float>(e.GetWidth()) / e.GetHeight();
		mProjection = glm::ortho(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, mZoomLevel, -mZoomLevel);
		return true;
	}

	bool Camera::OnMouseScroll(MouseScrolledEvent& e)
	{
		mZoomLevel -= e.GetYOffset() * 0.25f;
		mZoomLevel = std::max(mZoomLevel, 0.25f);
		mProjection = glm::ortho(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, mZoomLevel, -mZoomLevel);
		return false;
	}
}