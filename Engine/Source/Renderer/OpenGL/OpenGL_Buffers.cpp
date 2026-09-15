#include "aio_pch.hpp"
#include "OpenGL_Buffers.hpp"

namespace aio
{
	OpenGL_VertexBuffer::OpenGL_VertexBuffer(uint32_t size)
	{
		glCreateVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);

		glCreateBuffers(1, &mVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		glBindVertexArray(0);
	}

	OpenGL_VertexBuffer::OpenGL_VertexBuffer(const void* data, uint32_t size)
	{
		glCreateVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);

		glCreateBuffers(1, &mVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

		glBindVertexArray(0);
	}

	OpenGL_VertexBuffer::~OpenGL_VertexBuffer()
	{
		glDeleteBuffers(1, &mVBO);
	}

	void OpenGL_VertexBuffer::Bind() const
	{
		glBindVertexArray(mVAO);
	}

	void OpenGL_VertexBuffer::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGL_VertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	/////////////////////////////////////////////////////
	///////////INDEX BUFFER//////////////////////////////
	/////////////////////////////////////////////////////

	OpenGL_IndexBuffer::OpenGL_IndexBuffer(uint32_t* indices, uint32_t count)
	{
		glCreateBuffers(1, &mID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGL_IndexBuffer::~OpenGL_IndexBuffer()
	{
		glDeleteBuffers(1, &mID);
	}

	void OpenGL_IndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	}

	void OpenGL_IndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/////////////////////////////////////////////////////
	///////////UNIFORM BUFFER////////////////////////////
	/////////////////////////////////////////////////////

	UniformBuffer::UniformBuffer(uint32_t block_size, uint32_t slot)
	{
		glCreateBuffers(1, &mID);
		glNamedBufferData(mID, block_size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, mID);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &mID);
	}

	void UniformBuffer::SetData(const void* data, uint32_t data_size)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 0, data_size, data);
	}

	void UniformBuffer::Bind(uint32_t binding)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mID);
	}

	/////////////////////////////////////////////////////
	///////////VERTEX ARRAY//////////////////////////////
	/////////////////////////////////////////////////////

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
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


	VertexArray::VertexArray()
	{
		mVertexBuffer = nullptr;
		mIndexBuffer = nullptr;

		glCreateVertexArrays(1, &mID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &mID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(mID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(mID);

		mVertexBuffer = vertexBuffer;
		mVertexBuffer->Bind();

		glBindVertexArray(0);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(mID);
		mIndexBuffer = indexBuffer;
		mIndexBuffer->Bind();
		glBindVertexArray(0);
	}

}