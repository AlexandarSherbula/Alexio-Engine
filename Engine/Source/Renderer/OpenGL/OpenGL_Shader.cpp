#include "aio_pch.hpp"
#include "OpenGL_Shader.hpp"
#include "Utils/FileUtils.hpp"

namespace aio
{
	static GLenum OpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
			return GL_FLOAT;
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
			return GL_INT;
		case ShaderDataType::Bool:
			return GL_BOOL;
		}

		AIO_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		AIO_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGL_Shader::OpenGL_Shader(const std::string& name, const std::filesystem::path& filepath, const Ref<VertexInput>& vertexInput)
	{
		mName = name;

		std::string source = ReadFromFiles(filepath);
		mShaderSource = PreProcess(source);

		Compile(vertexInput);
	}

	OpenGL_Shader::OpenGL_Shader(const std::string& name, const std::string& vertexFile, const std::string& pixelFile, const Ref<VertexInput>& vertexInput)
	{
		mName = name;

		mShaderSource[GL_VERTEX_SHADER] = ReadFromFiles(vertexFile);
		mShaderSource[GL_FRAGMENT_SHADER] = ReadFromFiles(pixelFile);

		Compile(vertexInput);
	}

	OpenGL_Shader::~OpenGL_Shader()
	{
		glDeleteProgram(mID);
	}

	void OpenGL_Shader::VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
	{
		switch (type)
		{
		case GL_BYTE: case GL_UNSIGNED_BYTE: case GL_SHORT: case GL_UNSIGNED_SHORT: case GL_INT: case GL_UNSIGNED_INT:
		{
			glVertexAttribIPointer(index, size, type, stride, pointer);
			break;
		}
		case GL_DOUBLE: 
		{
			glVertexAttribLPointer(index, size, type, stride, pointer);
			break;
		}
		default:
			glVertexAttribPointer(index, size, type, normalized, stride, pointer);
		}
	}

	void OpenGL_Shader::Compile(const Ref<VertexInput>& vertexInput)
	{
		AIO_ASSERT(vertexInput->GetVertexBuffer()->GetLayout().GetElements().size(), "VertexBuffer has no layout");
		vertexInput->Bind();

		uint32_t index = 0;
		auto& layout = vertexInput->GetVertexBuffer()->GetLayout();
		for (auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			VertexAttribPointer(index,
				element.GetComponentCount(),
				OpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.offset);
			
			index++;
		}

		vertexInput->Unbind();

		GLuint program = glCreateProgram();

		std::array<GLenum, 2> glShaderIDs;
		uint8_t glShaderIDIndex = 0;
		for (auto& kv : mShaderSource)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;
			// Create an empty vertex shader handle
			GLuint shader = glCreateShader(type);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceC = source.c_str();
			glShaderSource(shader, 1, &sourceC, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				GLchar* message = (GLchar*)alloca(maxLength * sizeof(GLchar));
				glGetShaderInfoLog(shader, maxLength, &maxLength, message);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				const char* ch_type = (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
				AIO_LOG_ERROR("{0} shader compilation failed!", ch_type);
				AIO_LOG_ERROR("{0}", message);
				AIO_DEBUG_BREAK();
			}
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		mID = program;

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int32_t*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			AIO_LOG_ERROR("{0}", infoLog.data());
			AIO_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
			glDetachShader(program, id);
	}

	void OpenGL_Shader::Bind() const
	{
		glUseProgram(mID);
	}

	void OpenGL_Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGL_Shader::SetInt(const std::string& name, int32_t value)
	{
		glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
	}

	void OpenGL_Shader::SetIntArray(const std::string& name, int32_t* values, uint32_t count)
	{
		glUniform1iv(glGetUniformLocation(mID, name.c_str()), count, values);
	}

	void OpenGL_Shader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
	}

	void OpenGL_Shader::SetFloat2(const std::string& name, const Vector2& value)
	{
		glUniform2f(glGetUniformLocation(mID, name.c_str()), value.x, value.y);
	}

	void OpenGL_Shader::SetFloat3(const std::string& name, const Vector3& value)
	{
		glUniform3f(glGetUniformLocation(mID, name.c_str()), value.x, value.y, value.z);
	}

	void OpenGL_Shader::SetFloat4(const std::string& name, const Vector4& value)
	{
		glUniform4f(glGetUniformLocation(mID, name.c_str()), value.x, value.y, value.z, value.w);
	}

	void OpenGL_Shader::SetMat3x3(const std::string& name, const glm::mat3x3& matrix)
	{
		glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGL_Shader::SetMat4x4(const std::string& name, const Mat4x4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}	

	std::unordered_map<GLenum, std::string> OpenGL_Shader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			AIO_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			AIO_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}
}


