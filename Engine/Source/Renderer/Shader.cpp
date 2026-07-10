#include "aio_pch.hpp"
#include "Shader.hpp"

#include "Renderer.hpp"

#include "OpenGL/OpenGL_Shader.hpp"
#include "DX11/DX11_Shader.hpp"

#include "Utils/Timer.hpp"

namespace aio
{
	Slang::ComPtr<slang::IGlobalSession> SlangCompiler::sGlobalSession;
	std::unordered_map<std::string, Ref<Shader>> Shader::sShaders;

	static const char* PrintKind(slang::TypeReflection::Kind kind);
	static void PrintParameterLayout(slang::VariableLayoutReflection* varLayout);
	static std::string GetSampledTexture();

	void SlangCompiler::Run(const std::filesystem::path& slangFile, const std::string& slangSource, const std::string& name)
	{
		createGlobalSession(sGlobalSession.writeRef());

		slang::SessionDesc sessionDesc = {};
		slang::TargetDesc targetDesc = {};

		auto profileName = []()
			{
				CHECK_API(return "glsl_450", return "sm_5_0");
				return "";
			};

		auto targetFormat = []()
			{
				CHECK_API(return SLANG_GLSL, return SLANG_DXBC);
				return SLANG_TARGET_UNKNOWN;
			};

		targetDesc.format = targetFormat();
		targetDesc.profile = sGlobalSession->findProfile(profileName());

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Slang::ComPtr<slang::ISession> session;
		sGlobalSession->createSession(sessionDesc, session.writeRef());

		Slang::ComPtr<slang::IModule> slangModule;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			slangModule = session->loadModuleFromSourceString(
				name.c_str(),
				slangFile.string().c_str(),
				slangSource.c_str(),
				diagnosticsBlob.writeRef());
			DiagnoseIfNeeded(diagnosticsBlob);
			AIO_ASSERT(slangModule, "Failed to create a slang module");
		}

		Slang::ComPtr<slang::IEntryPoint> vsEntry, psEntry;
		slangModule->findEntryPointByName("VSMain", vsEntry.writeRef());
		AIO_ASSERT(vsEntry, "Failed to get Vertex Shader entry point");

		slangModule->findEntryPointByName("PSMain", psEntry.writeRef());
		AIO_ASSERT(psEntry, "Failed to get Pixel Shader entry point");

		std::array<slang::IComponentType*, 3> componentTypes = { slangModule, vsEntry, psEntry };
		Slang::ComPtr<slang::IComponentType> slangProgram;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			SlangResult result = session->createCompositeComponentType(
				componentTypes.data(),
				componentTypes.size(),
				slangProgram.writeRef(),
				diagnosticsBlob.writeRef());
			DiagnoseIfNeeded(diagnosticsBlob);
			AIO_ASSERT(SLANG_SUCCEEDED(result), "Failed to create a program");
		}

		Slang::ComPtr<slang::IComponentType> linkedProgram;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			SlangResult result = slangProgram->link(
				linkedProgram.writeRef(),
				diagnosticsBlob.writeRef());
			DiagnoseIfNeeded(diagnosticsBlob);
			AIO_ASSERT(SLANG_SUCCEEDED(result), "Failed to link a program");
		}

		Slang::ComPtr<slang::IBlob> vsCode;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			linkedProgram->getEntryPointCode(0, 0, vsCode.writeRef(), diagnosticsBlob.writeRef());
			SlangResult result = linkedProgram->getEntryPointCode(
				0, // entry point index
				0, // target index
				vsCode.writeRef(),
				diagnosticsBlob.writeRef());
			DiagnoseIfNeeded(diagnosticsBlob);
			AIO_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Vertex Shader code blob");
		}

		Slang::ComPtr<slang::IBlob> psCode;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			linkedProgram->getEntryPointCode(1, 0, psCode.writeRef(), diagnosticsBlob.writeRef());
			SlangResult result = linkedProgram->getEntryPointCode(
				1, // entry point index
				0, // target index
				psCode.writeRef(),
				diagnosticsBlob.writeRef());
			DiagnoseIfNeeded(diagnosticsBlob);
			AIO_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Pixel Shader code blob");
		}

		AIO_LOG_INFO("Vertex Shader succesfully compiled to {0} bytes of binary", vsCode->getBufferSize());
		AIO_LOG_INFO("Pixel Shader succesfully compiled to {0} bytes of binary", psCode->getBufferSize());

		auto writeBlobToFile = [](const std::filesystem::path& filepath, slang::IBlob* blob)
			{
				std::filesystem::create_directories(GetShaderCacheDirectory());

				std::ofstream file(filepath, std::ios::binary);
				if (!file)
					return false;

				file.write(
					reinterpret_cast<const char*>(blob->getBufferPointer()),
					static_cast<std::streamsize>(blob->getBufferSize())
				);

				return file.good();
			};

		auto writeSlangToCacheFile = [](const std::filesystem::path& filepath, const std::string& source)
			{
				std::filesystem::create_directories(GetShaderCacheDirectory());

				std::ofstream file(filepath, std::ios::binary);
				if (!file)
					return false;

				file.write(
					source.c_str(),
					static_cast<std::streamsize>(source.size())
				);

				return file.good();
			};

		writeBlobToFile(GetVertexShaderCacheFilePath(name), vsCode);
		writeBlobToFile(GetPixelShaderCacheFilePath(name), psCode);

		writeSlangToCacheFile(GetShaderCacheDirectory() / std::filesystem::path(name + ".cache"), slangSource);

		/////////////////////////////////////////////////////
		//////////////REFLECTION/////////////////////////////
		/////////////////////////////////////////////////////

		printf("\n");
		AIO_LOG_TRACE("SLANG COMPILING REFLECTION");
		printf("\n");
		AIO_LOG_TRACE("Global Parameters");
		slang::ProgramLayout* layout = linkedProgram->getLayout(0);
		int32_t paramCount = layout->getParameterCount();

		for (int32_t i = 0; i < paramCount; ++i)
		{
			auto param = layout->getParameterByIndex(i);
			const char* name = param->getName();
			AIO_LOG_INFO(" parameter name: {0}", name);
			PrintParameterLayout(param);
		}

		printf("\n");
		AIO_LOG_TRACE("Entry Point Parameters");
		int32_t entryCount = layout->getEntryPointCount();
		for (int32_t ei = 0; ei < entryCount; ++ei)
		{
			auto entry = layout->getEntryPointByIndex(ei);
			AIO_LOG_INFO(" entry: {0}", entry->getName());
			int usedParams = entry->getParameterCount();
			for (int pi = 0; pi < usedParams; ++pi)
			{
				auto p = entry->getParameterByIndex(pi);
				AIO_LOG_INFO(" param: {0}", p->getName());
				PrintParameterLayout(p);
			}
		}
	}

	std::filesystem::path SlangCompiler::GetVertexShaderCacheFilePath(const std::string& shaderName)
	{
		std::string fileExtension;

		CHECK_API
		(
			fileExtension = ".vert",
			fileExtension = ".cso"
		);

		return SlangCompiler::GetShaderCacheDirectory() / std::filesystem::path(shaderName + "-vs" + fileExtension);
	}

	std::filesystem::path SlangCompiler::GetPixelShaderCacheFilePath(const std::string& shaderName)
	{
		std::string fileExtension;

		CHECK_API
		(
			fileExtension = ".frag",
			fileExtension = ".cso"
		);

		return SlangCompiler::GetShaderCacheDirectory() / std::filesystem::path(shaderName + "-ps" + fileExtension);
	}

	void SlangCompiler::DiagnoseIfNeeded(slang::IBlob* diagnosticsBlob)
	{
		if (diagnosticsBlob != nullptr)
		{
			AIO_LOG_ERROR((const char*)diagnosticsBlob->getBufferPointer());
		}
	}

	Ref<Shader> Shader::Create(const std::filesystem::path& filepath, const Ref<VertexInput>& vertexInput, std::string name)
	{
		if (std::filesystem::exists(filepath))
		{
			if (name == "")
				name = GetFileName(filepath);

			if (filepath.string().find(".slang") != std::string::npos)
			{
				std::filesystem::path vsPath = SlangCompiler::GetVertexShaderCacheFilePath(name);
				std::filesystem::path psPath = SlangCompiler::GetPixelShaderCacheFilePath(name);

				std::string shaderSource = ReadFromFiles(filepath);

				if (shaderSource.find("GetSampledTexture") != std::string::npos)
				{
					std::string SampleTextureFunctionDefintion = GetSampledTexture();

					if (Renderer::CheckAPI() == GraphicsAPI::OpenGL && shaderSource.find("Texture2D") != std::string::npos)
					{
						std::string typedefAdd = "typedef Sampler2D Texture2D;\n";
						shaderSource = typedefAdd + shaderSource;
					}

					shaderSource += "\n" + SampleTextureFunctionDefintion;
				}

				auto HasShaderChanged = [&](std::string currentSource)
					{
						std::filesystem::path cachePath = SlangCompiler::GetShaderCacheDirectory() / std::filesystem::path(name + ".cache");

						if (!std::filesystem::exists(cachePath))
						{
							return true;
						}

						std::string cachedSource = ReadFromFiles(cachePath);

						currentSource.erase(std::remove_if(currentSource.begin(), currentSource.end(), [](unsigned char c)
							{
								return std::isspace(c);
							}), currentSource.end());

						cachedSource.erase(std::remove_if(cachedSource.begin(), cachedSource.end(), [](unsigned char c)
							{
								return std::isspace(c);
							}), cachedSource.end());

						return currentSource != cachedSource;
					};

				if (!std::filesystem::exists(vsPath) || !std::filesystem::exists(psPath) || HasShaderChanged(shaderSource))
				{
					SlangCompiler::Run(filepath, shaderSource, name);
				}

				return Shader::Create(name, vsPath.string(), psPath.string(), vertexInput);
			}

			CHECK_API
			(
				return CreateRef<OpenGL_Shader>(name, filepath, vertexInput),
				return CreateRef<DX11_Shader>(name, filepath, vertexInput)
			);
			return nullptr;
		}
		else
		{
			AIO_LOG_ERROR("Filepath '{0}' doesn't exist", filepath.string());
			AIO_DEBUG_BREAK();
			return nullptr;
		}
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexFile, const std::string& pixelFile, const Ref<VertexInput>& vertexInput)
	{
		CHECK_API
		(
			return CreateRef<OpenGL_Shader>(name, vertexFile, pixelFile, vertexInput),
			return CreateRef<DX11_Shader>(name, vertexFile, pixelFile, vertexInput)
		);
		return nullptr;
	}

	Ref<Shader> Shader::CreateAsset(const std::string& shaderFile, const Ref<VertexInput>& vertexInput, std::string name)
	{
		if (name == "")
			name = GetFileName(shaderFile);

		std::filesystem::path shaderFilePath = ASSETS_DIRECTORY / "shaders" / shaderFile;

		auto shader = Create(shaderFilePath, vertexInput, name);
		Shader::Add(shader, name);
		return shader;
	}

	Ref<Shader> Shader::Get(const std::string& name)
	{
		AIO_ASSERT(Exists(name), "Shader doesn't exist!");
		return sShaders[name];
	}

	void Shader::Add(const Ref<Shader>& shader, const std::string& name)
	{
		AIO_ASSERT(!Exists(name), "Shader already exists!");
		sShaders[name] = shader;
	}

	bool Shader::Exists(const std::string& name)
	{
		return sShaders.find(name) != sShaders.end();
	}

	std::string GetSampledTexture()
	{
		CHECK_API
		(
			return 
			"float4 GetSampledTexture(float2 texCoord, int texIndex)"
			"{"
			"	return textures[texIndex].Sample(texCoord);"
			"}",

			return 
			"float4 GetSampledTexture(float2 texCoord, int texIndex)"
			"{"
			"	return textures[texIndex].Sample(samplerState, texCoord);"
			"}"
		);

		return "";
	}
	
	const char* PrintKind(slang::TypeReflection::Kind kind)
	{
		typedef slang::TypeReflection::Kind Kind;

		switch (kind)
		{
		case Kind::None:                        return "None";
		case Kind::Struct:                      return "Struct";
		case Kind::Array:                       return "Array";
		case Kind::Matrix:                      return "Matrix";
		case Kind::Vector:                      return "Vector";
		case Kind::Scalar:                      return "Scalar";
		case Kind::ConstantBuffer:              return "ConstantBuffer";
		case Kind::Resource:                    return "Resource";
		case Kind::SamplerState:                return "SamplerState";
		case Kind::TextureBuffer:               return "TextureBuffer";
		case Kind::ShaderStorageBuffer:         return "ShaderStorageBuffer";
		case Kind::ParameterBlock:              return "ParameterBlock";
		case Kind::GenericTypeParameter:        return "GenericTypeParameter";
		case Kind::Interface:                   return "Interface";
		case Kind::OutputStream:                return "OutputStream";
		case Kind::Specialized:                 return "Specialized";
		case Kind::Feedback:                    return "Feedback";
		case Kind::Pointer:                     return "Pointer";
		case Kind::DynamicResource:             return "DynamicResource";
		case Kind::MeshOutput:                  return "MeshOutput";
		}

		return "";
	};

	void PrintParameterLayout(slang::VariableLayoutReflection* varLayout)
	{
		auto typeLayout = varLayout->getTypeLayout();
		AIO_LOG_TRACE("  kind: {0}", PrintKind(typeLayout->getKind()));

		int32_t catCount = varLayout->getCategoryCount();
		for (int32_t ci = 0; ci < catCount; ++ci)
		{
			auto cat = varLayout->getCategoryByIndex(ci);
			size_t slot = varLayout->getOffset(cat);
			AIO_LOG_TRACE("  binding slot: {0}", slot);
		}

		switch (typeLayout->getKind())
		{
			case slang::TypeReflection::Kind::Array:
			{
				AIO_LOG_TRACE("  array element size: {0}", typeLayout->getElementCount());
				if (auto elemLayout = typeLayout->getElementTypeLayout())
				{
					AIO_LOG_TRACE("    element kind: {0}", PrintKind(elemLayout->getKind()));
				}
				break;
			}
			case slang::TypeReflection::Kind::Struct:
			{
				int32_t fieldCount = typeLayout->getFieldCount();
				AIO_LOG_TRACE("  Struct fields:");
				for (int f = 0; f < fieldCount; ++f)
				{
					auto field = typeLayout->getFieldByIndex(f);
					auto name = field->getName();
					AIO_LOG_INFO("   name: {0}", name);
					PrintParameterLayout(field);
				}
				break;
			}
			case slang::TypeReflection::Kind::ConstantBuffer:
			case slang::TypeReflection::Kind::ParameterBlock:
			case slang::TypeReflection::Kind::TextureBuffer:
			case slang::TypeReflection::Kind::ShaderStorageBuffer:
			{
				auto elementVarLayout = typeLayout->getElementVarLayout();
				if (elementVarLayout)
				{
					auto elemTypeLayout = elementVarLayout->getTypeLayout();
					size_t size = elemTypeLayout->getSize();
					AIO_LOG_TRACE("  field kind: {0}", PrintKind(elemTypeLayout->getKind()));
					AIO_LOG_TRACE("  size={0}", size);
				}
				break;
			}
		}
	};
}


