#pragma once

#include "Camera.hpp"
#include "Utils/Math.hpp"

namespace aio
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float aspectRatio);

        void OnMove();
        void OnUpdate();
        void OnEvent(Event& e);
        bool OnMouseScroll(MouseScrolledEvent& e);

        Mat4x4 GetView() const { return mView; }
        Mat4x4 GetViewProjection() const { return mViewProjection; }

    private:
        float mZoomLevel;
        glm::vec2 mPosition;
        float mRotation;

        Mat4x4 mView;
        Mat4x4 mViewProjection;
    };
}