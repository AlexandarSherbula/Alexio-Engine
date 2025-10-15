#pragma once

#include "Renderer/Shader.hpp"

namespace aio
{
	class OpenGL_Shader : public Shader
	{
	public:
		OpenGL_Shader(const std::string& name, const Ref<VertexInput>& vertexInput);
		OpenGL_Shader(const std::string& name, const std::string& filepath, const Ref<VertexInput>& vertexInput);
		OpenGL_Shader(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc, const Ref<VertexInput>& vertexInput);
		~OpenGL_Shader();

		void Bind() const override;
		void Unbind() const override;

		void SetInt(const std::string& name, int32_t value) override;
		void SetIntArray(const std::string& name, int32_t* values, uint32_t count) override;
		void SetFloat(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, const Vector2& value) override;
		void SetFloat3(const std::string& name, const Vector3& value) override;
		void SetFloat4(const std::string& name, const Vector4& value) override;

		void SetMat3x3(const std::string& name, const glm::mat3x3& matrix) override;
		void SetMat4x4(const std::string& name, const Mat4x4& matrix) override;
	private:
		void Compile(const Ref<VertexInput>& vertexBuffer);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	private:
		uint32_t mID;
		std::unordered_map<GLenum, std::string> mShaderSource;
	};
}