#include "OpenGLShader.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLDevice.h"
#include "OpenGLView.h"
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Core/Exception.h>
#include <Core/Loger.h>
#include <fstream>
#include <iterator>

namespace RcEngine {

enum GLSLVersion
{
	GLSL420 = 0,
	GLSL430,
	GLSL440
};

static String GLSLVersion[] = {
	"#version 420\n",
	"#version 430\n",
	"#version 440\n",
};

const String& GetSupportedGLSLVersion() 
{
	if (GLEW_VERSION_4_4)
		return GLSLVersion[GLSL440];
	else if (GLEW_VERSION_4_3)
		return GLSLVersion[GLSL430]; 
	else if (GLEW_VERSION_4_2)
		return GLSLVersion[GLSL420]; 
	else
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Only supported OpenGL 4.2 above hardware!", "GetSupportedGLSLVersion");
}

bool HasVersion(const String& source)
{
	return source.find("#version") != std::string::npos;
}

inline bool LoadBinary(const char* filename, GLenum & format, std::vector<uint8_t>& bytecode)
{
	FILE* fp = fopen(filename, "rb");
	if(fp)
	{
		fread(&format, sizeof(GLenum), 1, fp);

		GLint size;
		fread(&size, sizeof(GLint), 1, fp);

		bytecode.resize(size);
		fread(&bytecode[0], size, 1, fp);

		fclose(fp);
		return true;
	}
	return false;
}

class OpenGLShaderReflection
{
public:
	OpenGLShaderReflection(OpenGLShader* shaderOGL)
		: mShaderOGL(shaderOGL)
	{
		assert(mShaderOGL);
		mShaderProgramID = mShaderOGL->mShaderOGL;
	}

	void ReflectShader()
	{
		if (mShaderOGL->GetShaderType() == ST_Vertex)
			RefectInputParameters();

		ReflectResource();
	}

private:
	void RefectInputParameters()
	{
		enum { MaxNameLen = 256 };
		GLchar name[MaxNameLen];

		GLint size;
		GLsizei actualLen;
		GLenum type;		

		GLint numAttribsInProgram;
		glGetProgramiv(mShaderProgramID, GL_ACTIVE_ATTRIBUTES, &numAttribsInProgram);
		for (GLuint i = 0; i < GLuint(numAttribsInProgram); ++i) 
		{
			glGetActiveAttrib(mShaderProgramID, i, MaxNameLen, &actualLen, &size, &type, name);

			InputSignature attribute;
			attribute.AttributeSlot = i;
			attribute.Type = type;
			attribute.ArraySize = size;

			mShaderOGL->mInputSignatures.push_back(attribute);
		}
	}

	void ReflectResource()
	{
		enum { MaxNameLen = 256 };
		GLchar name[MaxNameLen];
		
		GLint arraySize, arrayStride, matrixStride, unifomOffset, blockSize;;
		GLsizei actualNameLen;
		GLenum type;

		GLint numUniforms;
		glGetProgramiv(mShaderProgramID, GL_ACTIVE_UNIFORMS, &numUniforms);
		for (GLuint i = 0; i < GLuint(numUniforms); ++i)
		{
			glGetActiveUniform(mShaderProgramID, i, MaxNameLen, &actualNameLen, &arraySize, &type, name);
			/**
			 * Hack:
			 * OpenGL seems to treat const variable as active uniform with a modified name.
			 * Don't retrieve it.
			 */
			 if (!isalpha(name[0]))
				 continue;

			if (arraySize > 1 && actualNameLen > 3) // Check array type may contain []
			{
				// remove [] if exits
				actualNameLen = std::distance(name, std::find(name, name + actualNameLen, '['));
			}

			// Variable name
			String actualName(&name[0], actualNameLen);

			// Get uniform block for this uniform
			GLint blockIdx;
			glGetActiveUniformsiv(mShaderProgramID, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIdx);
			if (blockIdx == GL_INVALID_INDEX) 
			{
				// Global uniform, SRV or UAV
				OpenGLShaderParameterClass paramClass;
				EffectParameterType paramType;
				OpenGLMapping::UnMapping(type, paramType, paramClass);
					
				if (paramClass == Shader_Param_Uniform)
				{
					UniformParam uniform;

					uniform.Name = actualName;
					uniform.Type = paramType;
					uniform.ArraySize = arraySize;
					uniform.Location = i;
					assert( i == glGetProgramResourceLocation(mShaderProgramID, GL_UNIFORM, &name[0]) );

					mShaderOGL->mGlobalParams.push_back(uniform);
				}
				else if (paramType == Shader_Param_SRV)
				{
					// Texture or TBuffer
					SRVParam srvParam;
					srvParam.Name = actualName;
					srvParam.Type = paramType;
					srvParam.Location = i;
					assert( i == glGetProgramResourceLocation(mShaderProgramID, GL_UNIFORM, &name[0]) );

					mShaderOGL->mSRVParams.push_back(srvParam);
				}
				else 
				{
					// Image or ImageBuffer
					UAVParam uavParam;
					uavParam.Name = actualName;
					uavParam.Type = paramType;
					uavParam.Location = i;
					assert( i == glGetProgramResourceLocation(mShaderProgramID, GL_UNIFORM, &name[0]) );

					mShaderOGL->mUAVParams.push_back(uavParam);
				}
			}
		}

		// Uniform blocks
		GLint numUniformBlocks;
		glGetProgramiv(mShaderProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);
		for (GLuint i = 0; i < GLuint(numUniformBlocks); ++i)
		{
			glGetActiveUniformBlockName(mShaderProgramID, i, 256, NULL, name);
			glGetActiveUniformBlockiv(mShaderProgramID, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
			
			// binding point
			GLint binding;
			glGetActiveUniformBlockiv(mShaderProgramID, i,  GL_UNIFORM_BLOCK_BINDING, &binding);

			GLint numUniformInBlock;
			glGetActiveUniformBlockiv(mShaderProgramID, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniformInBlock);

			std::vector<GLuint> indices(numUniformInBlock);
			glGetActiveUniformBlockiv(mShaderProgramID, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)&indices[0]);

			UniformBufferParam uniformBufferParam;
			uniformBufferParam.Name = String(name, actualNameLen);
			uniformBufferParam.Location = i;

			for (int k = 0; k < numUniformInBlock; ++k) 
			{
				glGetActiveUniform(mShaderProgramID, i, MaxNameLen, &actualNameLen, &arraySize, &type, name);	
				glGetActiveUniformsiv(mShaderProgramID, 1, &indices[k], GL_UNIFORM_OFFSET, &unifomOffset);
				glGetActiveUniformsiv(mShaderProgramID, 1, &indices[k], GL_UNIFORM_ARRAY_STRIDE, &arrayStride);
				glGetActiveUniformsiv(mShaderProgramID, 1, &indices[k], GL_UNIFORM_MATRIX_STRIDE, &matrixStride);

				// Check array type may contain []
				if (arraySize > 1 && actualNameLen > 3) 
				{
					// remove [] if exits
					actualNameLen = std::distance(name, std::find(name, name + actualNameLen, '['));
				}

				UniformParam bufferVariable;
				bufferVariable.Name = String(name, actualNameLen);
				bufferVariable.ArraySize = arraySize;
				
				OpenGLShaderParameterClass paramClass;
				OpenGLMapping::UnMapping(type, bufferVariable.Type, paramClass);
				assert(paramClass == Shader_Param_Uniform);

				uniformBufferParam.BufferVariables.push_back(bufferVariable);
			}

			mShaderOGL->mUniformBuffers.push_back(uniformBufferParam);
		}

		// Shader Storage Blocks
		GLint numSSBO;
		glGetProgramInterfaceiv(mShaderProgramID, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numSSBO);
		for (GLuint i = 0; i < GLuint(numSSBO); ++i)
		{
			glGetProgramResourceName(mShaderProgramID, GL_SHADER_STORAGE_BLOCK, i, MaxNameLen, &actualNameLen, name);
			
			String actualName(name, actualNameLen);
			if (actualName.find("SRV") != String::npos)
			{
				SRVParam srvParam;
				srvParam.Name = actualName.substr(0, actualName.find("SRV"));
				srvParam.Type = EPT_StructureBuffer;
				srvParam.Location = i;

				mShaderOGL->mSRVParams.push_back(srvParam);
			}
			else if (actualName.find("UAV") != String::npos)
			{
				UAVParam uavParam;
				uavParam.Name = actualName.substr(0, actualName.find("UAV"));
				uavParam.Type = EPT_StructureBuffer;
				uavParam.Location = i;

				mShaderOGL->mUAVParams.push_back(uavParam);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "GLSL StructureBuffer must be postfix with SRV or UAV", "ReflectResource");
			}
		}
	}

private:
	OpenGLShader* mShaderOGL;
	GLuint mShaderProgramID;
};

template <typename T>
struct ShaderParameterSetHelper {};

template<>
struct ShaderParameterSetHelper<bool>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			UpdateTimeStamp = Param->GetTimeStamp();

			bool value;  Param->GetValue(value);
			glProgramUniform1i(ShaderOGL, Location, value);
		} 
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template<>
struct ShaderParameterSetHelper<bool*>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(Location), Param(param), UpdateTimeStamp(0), Count(count) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			UpdateTimeStamp = Param->GetTimeStamp();

			assert(Param->GetElementSize() == Count);

			bool* pValue;
			Param->GetValue(pValue);

			vector<GLint> temp(Count);
			for (GLsizei i = 0; i < Count; ++i)
				temp[i] = pValue[i];

			glProgramUniform1iv(ShaderOGL, Location, Count, &temp[0]);
		} 
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<int32_t>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0)  { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			UpdateTimeStamp = Param->GetTimeStamp();

			int32_t value;  Param->GetValue(value);
			glProgramUniform1i(ShaderOGL, Location, value);
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<int32_t*>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(location), Param(param), Count(count), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			int32_t* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glProgramUniform1iv(ShaderOGL, Location, Count, pValue);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float value; Param->GetValue(value);
			glProgramUniform1f(ShaderOGL, Location, value);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float*>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glProgramUniform1fv(ShaderOGL, Location, Count, pValue);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float2>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float2 value; Param->GetValue(value);
			glProgramUniform2f(ShaderOGL, Location, value.X(), value.Y());

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float2*>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float2* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glProgramUniform2fv(ShaderOGL, Location, Count,  reinterpret_cast<float*>(pValue));

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float3>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float3 value; Param->GetValue(value);
			glProgramUniform3f(ShaderOGL, Location, value[0], value[1], value[2]);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float3*>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float3* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glProgramUniform3fv(ShaderOGL, Location, Count,  reinterpret_cast<float*>(pValue));

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4 value; Param->GetValue(value);
			glProgramUniform4f(ShaderOGL, Location, value[0], value[1], value[2], value[3]);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4*>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glProgramUniform4fv(ShaderOGL, Location, Count,  reinterpret_cast<float*>(pValue));

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};


template <>
struct ShaderParameterSetHelper<float4x4>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4x4 value; 
			Param->GetValue(value);

			// we know that glsl matrix is column major, so we need transpose out matrix.
			glProgramUniformMatrix4fv(ShaderOGL, Location, 1, true, &value[0]);
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4x4*>
{
public:
	ShaderParameterSetHelper(GLuint shaderID, GLint location, EffectParameter* param, GLsizei count)
		: ShaderOGL(shaderID), Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4x4* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glProgramUniformMatrix4fv(ShaderOGL, Location, Count, true, reinterpret_cast<float*>(pValue));
		}
	}

private:
	GLuint ShaderOGL;
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template<>
struct ShaderParameterSetHelper<RHShaderResourceView>
{
	ShaderParameterSetHelper(EffectParameter* param, GLuint binding)
		: Param(Param), Binding(binding) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			weak_ptr<RHShaderResourceView> srv;
			Param->GetValue(srv);

			if (auto spt = srv.lock())
			{
				OpenGLShaderResourceView* srvOGL = static_cast_checked<OpenGLShaderResourceView*>(spt.get());
				srvOGL->BindSRV(Binding);
			}
		}
	}

private:
	GLuint Binding;
	EffectParameter* Param;
	TimeStamp UpdateTimeStamp;
};

template<>
struct ShaderParameterSetHelper<RHUnorderedAccessView>
{
	ShaderParameterSetHelper(EffectParameter* param, GLuint binding)
		: Param(Param), Binding(binding) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			weak_ptr<RHUnorderedAccessView> uav;
			Param->GetValue(uav);

			if (auto spt = uav.lock())
			{
				OpenGLUnorderedAccessView* srvOGL = static_cast_checked<OpenGLUnorderedAccessView*>(spt.get());
				srvOGL->BindUAV(Binding);
			}
		}
	}

private:
	GLuint Binding;
	EffectParameter* Param;
	TimeStamp UpdateTimeStamp;
};



struct ShaderTextureBinding
{
	ShaderTextureBinding(GLuint shaderID, GLint location, GLuint binding)
		: ShaderOGL(shaderID), Location(location), Binding(binding) {}
	
	void operator() ()
	{
		glProgramUniform1i(ShaderOGL, Location, Binding);
	}

private:
	GLuint ShaderOGL;
	GLuint Binding;
	GLuint Location;
};

struct ShaderStorageBinding
{
	ShaderStorageBinding(GLuint shaderID, GLint location, GLuint binding)
		: ShaderOGL(shaderID), Location(location), Binding(binding) {}

	void operator() ()
	{
		glShaderStorageBlockBinding(ShaderOGL, Location, Binding);
	}

private:
	GLuint ShaderOGL;
	GLuint Binding;
	GLuint Location;
};

//////////////////////////////////////////////////////////////////////////
OpenGLShader::OpenGLShader( ShaderType shaderType )
	: RHShader(shaderType),
	  mShaderOGL(0)
{

}

OpenGLShader::~OpenGLShader()
{
	if (mShaderOGL)
	{
		glDeleteProgram(mShaderOGL);
		mShaderOGL = 0;
	}
}

bool OpenGLShader::LoadFromByteCode( const String& filename )
{
	GLenum format;
	std::vector<uint8_t> byteCode;

	if ( LoadBinary(filename.c_str(), format, byteCode) == false )
	{
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	GLint success;

	mShaderOGL = glCreateProgram();
	glProgramBinary(mShaderOGL, format, &byteCode[0], byteCode.size());
	glProgramParameteri(mShaderOGL, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	glGetProgramiv(mShaderOGL, GL_LINK_STATUS, &success);

	if (success != GL_TRUE)
	{
		int length;
		glGetProgramiv(mShaderOGL, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> compileOutput(length);
		glGetProgramInfoLog(mShaderOGL, length, &length, &compileOutput[0]);

		EngineLoger::LogError("GLSL %s compile failed\n\n%s\n\n", filename.c_str(), &compileOutput[0]);
	}

	OGL_ERROR_CHECK();
	return (success == GL_TRUE);
}

bool OpenGLShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint /*= ""*/ )
{
	assert(GLEW_ARB_separate_shader_objects);

	String shaderSource;

	for (uint32_t i = 0; i < macroCount; ++i)
		shaderSource += "#define " + macros[i].Name + " " +  macros[i].Definition + "\r\n";

	// Add #line 1
	//shaderSource += "#line 1";

	// Load source code into string
	std::ifstream fileStream(filename);
	shaderSource.append((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

	if (HasVersion(shaderSource))
	{
		char const* pSource = shaderSource.c_str();

		mShaderOGL = glCreateShaderProgramv(OpenGLMapping::Mapping(mShaderType), 1, &pSource);
	}
	else
	{
		const char* pSource[2];

		const String& latestVersion = GetSupportedGLSLVersion();
		pSource[0] = latestVersion.c_str();
		pSource[1] = shaderSource.c_str();

		mShaderOGL = glCreateShaderProgramv(OpenGLMapping::Mapping(mShaderType), 2, pSource);
	}

	int success;
	glGetProgramiv(mShaderOGL, GL_LINK_STATUS, &success);

	if (success != GL_TRUE)
	{
		int length;
		glGetProgramiv(mShaderOGL, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> compileOutput(length);
		glGetProgramInfoLog(mShaderOGL, length, &length, &compileOutput[0]);

		EngineLoger::LogError("GLSL %s compile failed\n\n%s\n\n", filename.c_str(), &compileOutput[0]);
	}

	OpenGLShaderReflection shaderReflection(this);
	shaderReflection.ReflectShader();

	OGL_ERROR_CHECK();
	return (success == GL_TRUE);
}


//////////////////////////////////////////////////////////////////////////

OpenGLShaderPipeline::OpenGLShaderPipeline(Effect& effect)
	: RHShaderPipeline(effect)
{

}

void OpenGLShaderPipeline::OnBind()
{
	// Commit all shader parameter
	for (auto& paramBindFunc : mParameterBinds)
		paramBindFunc();

	if (mShaderStages[ST_Vertex])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Vertex].get()))->mShaderOGL;
		gOpenGLDevice->BindVertexShader(shaderOGL);
		// input layout
	}

	if (mShaderStages[ST_Hull])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Hull].get()))->mShaderOGL;
		gOpenGLDevice->BindTessControlShader(shaderOGL);
	}

	if (mShaderStages[ST_Domain])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Domain].get()))->mShaderOGL;
		gOpenGLDevice->BindTessEvalShader(shaderOGL);
	}

	if (mShaderStages[ST_Geomerty])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Geomerty].get()))->mShaderOGL;
		gOpenGLDevice->BindGeometryShader(shaderOGL);
	}

	if (mShaderStages[ST_Pixel])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Pixel].get()))->mShaderOGL;
		gOpenGLDevice->BindPixelShader(shaderOGL);
	}

	if (mShaderStages[ST_Compute])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Compute].get()))->mShaderOGL;
		gOpenGLDevice->BindComputeShader(shaderOGL);
	}
}

void OpenGLShaderPipeline::OnUnbind()
{

	if (mShaderStages[ST_Vertex])	gOpenGLDevice->BindVertexShader(0);
	if (mShaderStages[ST_Hull])		gOpenGLDevice->BindTessControlShader(0);
	if (mShaderStages[ST_Domain])	gOpenGLDevice->BindTessEvalShader(0);
	if (mShaderStages[ST_Geomerty])	gOpenGLDevice->BindGeometryShader(0);
	if (mShaderStages[ST_Pixel])	gOpenGLDevice->BindPixelShader(0);
	if (mShaderStages[ST_Compute])	gOpenGLDevice->BindComputeShader(0);
}

bool OpenGLShaderPipeline::LinkPipeline()
{
	GLuint srvBinding = 0;
	GLuint uavBinding = 0;

	for (int i = 0; i < ST_Count; ++i)
	{
		if (mShaderStages[i])
		{
			OpenGLShader* shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Vertex].get()));

			// Global uniforms
			for (const UniformParam& param : shaderOGL->mGlobalParams)
			{
				EffectParameter* uniformParam = mEffect.FetchUniformParameter(param.Name, param.Type, param.ArraySize);
				AddUniformParamBind(shaderOGL->mShaderOGL, param.Location, uniformParam, param.ArraySize);
			}

			// Uniform buffers
			for (const UniformBufferParam& param : shaderOGL->mUniformBuffers)
			{
				EffectUniformBuffer* uniformBuffer = mEffect.FetchUniformBufferParameter(param.Name, param.BufferSize);

				if (uniformBuffer->GetNumVariable() > 0)
				{
					// check buffer variables
					assert(uniformBuffer->GetNumVariable() == param.BufferVariables.size());
					for (size_t i = 0; i < param.BufferVariables.size(); ++i)
					{
						EffectParameter* variable = uniformBuffer->GetVariable(i);
						if (variable->GetName() != param.BufferVariables[i].Name ||
							variable->GetParameterType() != param.BufferVariables[i].Type ||
							variable->GetElementSize() != param.BufferVariables[i].ArraySize ||
							variable->GetOffset() != param.BufferVariables[i].Location)
						{
							ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Error: Same uniform buffer with different variables!", "D3D11ShaderPipeline::LinkPipeline");
						}
					}
				}
				else
				{
					for (const auto& bufferVariable : param.BufferVariables)
					{
						EffectParameter* variable = mEffect.FetchUniformParameter(bufferVariable.Name, bufferVariable.Type, bufferVariable.ArraySize);
						uniformBuffer->AddVariable(variable, bufferVariable.Location);
					}
				}
			}

			// Shader resource views
			for (const SRVParam& param : shaderOGL->mSRVParams)
			{
				if (mBindingCache.find(param.Name) == mBindingCache.end())
				{		
					mBindingCache[param.Name] = srvBinding++;

					EffectParameter* srvParam = mEffect.FetchSRVParameter(param.Name, param.Type);
					AddSRVParamBind(srvParam, mBindingCache[param.Name]);

				}

				AddShaderResourceBind(shaderOGL->mShaderOGL, param.Location, mBindingCache[param.Name], param.Type == EPT_StructureBuffer);
			}

			// Unordered access views
			for (const UAVParam& param : shaderOGL->mUAVParams)
			{
				if (mBindingCache.find(param.Name) == mBindingCache.end())
				{
					mBindingCache[param.Name] = uavBinding++;

					EffectParameter* uavParam = mEffect.FetchUAVParameter(param.Name, param.Type);
					AddUAVParamBind(uavParam, mBindingCache[param.Name]);
				}

				AddShaderResourceBind(shaderOGL->mShaderOGL, param.Location, mBindingCache[param.Name], param.Type == EPT_StructureBuffer);
			}

		}
	}

	return true;
}

void OpenGLShaderPipeline::AddUniformParamBind( GLuint shader, GLint location, EffectParameter* effectParam, GLsizei arrSize )
{
	switch(effectParam->GetParameterType())
	{
	case EPT_Boolean:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<bool*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<bool>(shader, location, effectParam) );
		}
		break;
	case EPT_Int:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<int32_t*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<int32_t>(shader, location, effectParam) );
		}
		break;
	case EPT_Float:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float>(shader, location, effectParam) );
		}
		break;
	case EPT_Float2:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float2*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float2>(shader, location, effectParam) );	
		}
		break;
	case EPT_Float3:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float3*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float3>(shader, location, effectParam) );	
		}
		break;
	case EPT_Float4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float4*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float4>(shader, location, effectParam) );		
		}
		break;
	case EPT_Matrix4x4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float4x4*>(shader, location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float4x4>(shader, location, effectParam) );		
		}
		break;
	default:
		assert(false);
	}
}

void OpenGLShaderPipeline::AddSRVParamBind( EffectParameter* effectParam, GLuint binding )
{
	mParameterBinds.push_back( ShaderParameterSetHelper<RHShaderResourceView>(effectParam, binding) );
}

void OpenGLShaderPipeline::AddUAVParamBind( EffectParameter* effectParam, GLuint binding )
{
	mParameterBinds.push_back( ShaderParameterSetHelper<RHUnorderedAccessView>(effectParam, binding) );
}

void OpenGLShaderPipeline::AddShaderResourceBind( GLuint shader, GLint location, GLuint binding, bool shaderStorage )
{
	if (shaderStorage)
	{
		mParameterBinds.push_back( ShaderStorageBinding(shader, location, binding) );
	}
	else
	{
		mParameterBinds.push_back( ShaderTextureBinding(shader, location, binding) );
	}
}



}