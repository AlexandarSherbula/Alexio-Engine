#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

namespace aio
{
	using Vector2 = glm::vec2;
	using Vector3 = glm::vec3;
	using Vector4 = glm::vec4;

	using Vector2i = glm::ivec2;
	using Vector3i = glm::ivec3;
	using Vector4i = glm::ivec4;

	using Mat2 = glm::mat2;
	using Mat2x2 = glm::mat2x2;

	using Mat2x3 = glm::mat2x3;
	using Mat2x4 = glm::mat2x4;
	using Mat3x2 = glm::mat3x2;

	using Mat3 = glm::mat3;
	using Mat3x3 = glm::mat3x3;

	using Mat3x4 = glm::mat3x4;
	using Mat4x2 = glm::mat4x2;
	using Mat4x3 = glm::mat4x3;

	using Mat4x4 = glm::mat4;
	using Mat4x4 = glm::mat4x4;

	float Length(const Vector2& v);
	float Length(const Vector3& v);
	float Length(const Vector4& v);

	template<typename T>
	inline T Length(T x) { return glm::length(x); }
	
	template<typename T>
	inline T Radians(T degrees) { return glm::radians(degrees); }

	template<typename T>
	inline T Degree(T radians) { return glm::degrees(radians); }

	std::ostream& operator<< (std::ostream& os, const Vector2& vec2);
	std::ostream& operator<< (std::ostream& os, const Vector3& vec3);
	std::ostream& operator<< (std::ostream& os, const Vector4& vec4);

	bool DecomposeTransform(const Mat4x4& transform, Vector3& translation, Vector3& rotation, Vector3& scale);
}