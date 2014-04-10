#include "OpenGLShaderProgram.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLSamplerState.h"
#include "OpenGLGraphicBuffer.h"
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/RenderDevice.h>
#include <Core/Exception.h>
#include <Core/Context.h>

namespace RcEngine {

template <typename T>
struct ShaderParameterSetHelper {};

template<>
struct ShaderParameterSetHelper<bool>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			UpdateTimeStamp = Param->GetTimeStamp();

			bool value;  Param->GetValue(value);
			glUniform1i(Location, value);
		} 
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template<>
struct ShaderParameterSetHelper<bool*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param, GLsizei count)
		: Location(Location), Param(param), UpdateTimeStamp(0), Count(count) {}

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

			glUniform1iv(Location, Count, &temp[0]);
		} 
	}

private:
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<int32_t>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0)  { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			UpdateTimeStamp = Param->GetTimeStamp();

			int32_t value;  Param->GetValue(value);
			glUniform1i(Location, value);
		}
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<int32_t*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param, GLsizei count)
		: Location(location), Param(param), Count(count), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			int32_t* pValue;
			Param->GetValue(pValue);
			
			assert(Count == Param->GetElementSize());
			glUniform1iv(Location, Count, pValue);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float value; Param->GetValue(value);
			glUniform1f(Location, value);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param, GLsizei count)
		: Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glUniform1fv(Location, Count, pValue);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float2>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float2 value; Param->GetValue(value);
			glUniform2f(Location, value.X(), value.Y());
			
			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float2*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param, GLsizei count)
		: Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float2* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glUniform2fv(Location, Count,  reinterpret_cast<float*>(pValue));

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float3>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float3 value; Param->GetValue(value);
			glUniform3f(Location, value[0], value[1], value[2]);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float3*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param, GLsizei count)
		: Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float3* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glUniform3fv(Location, Count,  reinterpret_cast<float*>(pValue));

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4 value; Param->GetValue(value);
			glUniform4f(Location, value[0], value[1], value[2], value[3]);

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param, GLsizei count)
		: Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glUniform4fv(Location, Count,  reinterpret_cast<float*>(pValue));

			UpdateTimeStamp = Param->GetTimeStamp();
		}
	}

private:
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};


template <>
struct ShaderParameterSetHelper<float4x4>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4x4 value; 
			Param->GetValue(value);

			// we know that glsl matrix is column major, so we need transpose out matrix.
			glUniformMatrix4fv(Location, 1, true, &value[0]);
		}
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4x4*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param, GLsizei count)
		: Location(location), Param(param), UpdateTimeStamp(0), Count(count) { }

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			float4x4* pValue;
			Param->GetValue(pValue);

			assert(Count == Param->GetElementSize());
			glUniformMatrix4fv(Location, Count, true, reinterpret_cast<float*>(pValue));
		}
	}

private:
	GLint Location;
	GLsizei Count;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};

// note that a texture object always need a sample state to sample it 
template<>
struct ShaderParameterSetHelper< TextureLayer >
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param), UpdateTimeStamp(0) { }

	void operator() ()
	{
		TextureLayer textureLayer; Param->GetValue(textureLayer);
		shared_ptr<OpenGLTexture> textureOGL = std::static_pointer_cast<OpenGLTexture>(textureLayer.Texture);

		if (textureOGL)
		{
			if (Param->GetTimeStamp() != UpdateTimeStamp)
			{
				RenderDevice& device = Context::GetSingleton().GetRenderDevice();
				device.SetSamplerState(textureLayer.Stage, textureLayer.TexUnit, textureLayer.Sampler);

				UpdateTimeStamp = Param->GetTimeStamp();
			}

			glActiveTexture(GL_TEXTURE0+textureLayer.TexUnit);
			glBindTexture(textureOGL->GetOpenGLTextureTarget(), textureOGL->GetOpenGLTexture());
			glUniform1i(Location, textureLayer.TexUnit);				
		}
	}

private:
	GLint Location;
	TimeStamp UpdateTimeStamp;
	EffectParameter* Param;
};


template<>
struct ShaderParameterSetHelper< EffectConstantBuffer >
{
public:
	ShaderParameterSetHelper(GLuint bindingSlot, EffectConstantBuffer* param)
		: BindingSlot(bindingSlot), UniformBlock(param) { }

	void operator() ()
	{
		// Update uniform buffer if changed
		UniformBlock->UpdateBuffer();

		shared_ptr<OpenGLGraphicsBuffer> uniformBuffer = std::static_pointer_cast<OpenGLGraphicsBuffer>(UniformBlock->GetBuffer()); 
		glBindBufferBase(GL_UNIFORM_BUFFER, BindingSlot, uniformBuffer->GetBufferID());
	}

private:
	GLuint BindingSlot;
	EffectConstantBuffer* UniformBlock;
};

template<>
struct ShaderParameterSetHelper<U>
{

};


//////////////////////////////////////////////////////////////////////////
OpenGLShaderProgram::OpenGLShaderProgram(Effect& effect)
	: ShaderProgram(effect), mOGLProgramObject(0)
{
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
	Release();
}

void OpenGLShaderProgram::Bind()
{
	glUseProgram(mOGLProgramObject);

	for (auto& paramBindFunc : mParameterBinds)
	{
		paramBindFunc();
		OGL_ERROR_CHECK();
	}
}

void OpenGLShaderProgram::Unbind()
{

}

bool OpenGLShaderProgram::LinkProgram()
{
	Release();

	if (mAttachedShaders.empty())
		return false;

	mOGLProgramObject = glCreateProgram();
	//printf("create shader program ID: %d \n", mOGLProgramObject);

	if (!mOGLProgramObject)
	{
		mLinkerOutput = "Could not create shader program";
		return false;
	}

	for (auto iter = mAttachedShaders.begin(); iter != mAttachedShaders.end(); ++iter)
	{
		shared_ptr<OpenGLShader> shaderOGL = std::static_pointer_cast<OpenGLShader>(*iter);
		glAttachShader(mOGLProgramObject, shaderOGL->GetShaderObject());
	}
	glLinkProgram(mOGLProgramObject);

	int linked;
	glGetProgramiv(mOGLProgramObject, GL_LINK_STATUS, &linked);
	mValidate = (linked != 0);
	if (!mValidate)
	{
		int length;
		glGetProgramiv(mOGLProgramObject, GL_INFO_LOG_LENGTH, &length);
		mLinkerOutput.resize(length);
		glGetProgramInfoLog(mOGLProgramObject, length, &length, &mLinkerOutput[0]);
	}
	else
	{
		mLinkerOutput.clear();
	}

	if (!mValidate)
		return false;

	CaptureAllParameter();

	OGL_ERROR_CHECK();

	return mValidate;
}

void OpenGLShaderProgram::Release()
{
	if (mOGLProgramObject)
	{
		for (auto iter = mAttachedShaders.begin(); iter != mAttachedShaders.end(); ++iter)
		{
			shared_ptr<OpenGLShader> shaderOGL = std::static_pointer_cast<OpenGLShader>(*iter);
			glDetachShader(mOGLProgramObject, shaderOGL->GetShaderObject());
		}

		mAttachedShaders.clear();
		mParameterBinds.clear();

		glDeleteProgram(mOGLProgramObject);
		mOGLProgramObject = 0;
	}
}

String GetGLSLTypeString(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
		return "float";
	case GL_FLOAT_VEC2:
		return "vec2";
	case GL_FLOAT_VEC3:
		return "vec3";
	case GL_FLOAT_VEC4:
		return "vec4";
	case GL_INT:
		return "int";
	case GL_INT_VEC2:
		return "int2";
	case GL_INT_VEC3:
		return "int3";
	case GL_INT_VEC4:
		return "int4";
	case GL_UNSIGNED_INT:
		return "uint";
	case GL_UNSIGNED_INT_VEC2:
		return "uint2";
	case GL_UNSIGNED_INT_VEC3:
		return "uint3";
	case GL_UNSIGNED_INT_VEC4:
		return "uint4";
	case GL_FLOAT_MAT2:
		return "mat2";
	case GL_FLOAT_MAT3:
		return "mat3";
	case GL_FLOAT_MAT4:
		return "mat4";
	default:
		break;
	}
	return "unknow";
}

void OpenGLShaderProgram::CaptureAllParameter()
{
	/*GLint activeAttribs;
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
	if (activeAttribs > 0)
	{
	GLint maxLength;
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	vector<char> maxName(maxLength);

	GLint length;
	GLint attibSize;
	GLenum attibType;
	unsigned int samplerIndex = 0;	
	}*/

	GLint size, location, maxNameLen;
	GLsizei nameLen;
	GLenum type;
	
	// Get max character length for active uniform maxNameLen
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);
	std::vector<GLchar> name(maxNameLen);
	
	// Active uniforms in each block
	std::vector<std::vector<const GLchar*>> blockVariableNames;
	std::vector<std::vector<EffectParameter*>> blockVariables;

	GLint numUniformsInProgram;
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORMS, &numUniformsInProgram);
	for (GLuint i = 0; i < GLuint(numUniformsInProgram); ++i)
	{
		glGetActiveUniform(mOGLProgramObject, i, maxNameLen, &nameLen, &size, &type, &name[0]);
		
		/**
		 * Hack:
		 * OpenGL seems to treat const variable as active uniform with a modified name.
		 * Don't retrieve it.
		 */
		 if (!isalpha(name[0]))
			 continue;

		// Check array type
		if (size > 1 && nameLen > 3) // must contain []
		{
			// Get variable name without []
			nameLen = std::distance(name.begin(), std::find(name.begin(), name.begin() + nameLen, '['));
		}

		// Variable name
		std::string actualName(&name[0], nameLen);

		// Variable type
		EffectParameterType effectParamType;
		OpenGLMapping::UnMapping(effectParamType, type);
		EffectParameter* effectParam = mEffect.FetchShaderParameter(actualName, effectParamType, size);

		// Get uniform block for this uniform
		GLint blockIdx;
		glGetActiveUniformsiv(mOGLProgramObject, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIdx);
		if (blockIdx != -1) //GL_INVALID_INDEX
		{
			// Uniform block
			if (blockVariableNames.size() < size_t(blockIdx + 1))
			{
				blockVariableNames.resize(blockIdx + 1);
				blockVariables.resize(blockIdx + 1);
			}

			blockVariables[blockIdx].push_back(effectParam);
			blockVariableNames[blockIdx].push_back(effectParam->GetName().c_str());
		}
		else
		{
			location = glGetUniformLocation(mOGLProgramObject, &name[0]);
			AddParameterBind(location, effectParam, size);
			effectParam->MakeDirty();
		}
	}

	GLint numBlocks;
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
	assert(blockVariableNames.size() == numBlocks);

	if (numBlocks > 0)
	{
		GLint blockSize, numUniformInBlock;
		std::vector<GLuint> indices;
		std::vector<GLint> offset;
		std::vector<GLint> types;
		std::vector<GLint> arraySize;
		std::vector<GLint> arrayStrides;
		std::vector<GLint> matrixStrides;

		for (GLuint i = 0; i < GLuint(blockVariableNames.size()); ++i)
		{
			glGetActiveUniformBlockiv(mOGLProgramObject, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &maxNameLen); 
			name.resize(maxNameLen);

			// Get uniform block name
			glGetActiveUniformBlockName(mOGLProgramObject, i, maxNameLen, &nameLen, &name[0]);

			glGetActiveUniformBlockiv(mOGLProgramObject, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniformInBlock); 
			assert(blockVariableNames[i].size() == numUniformInBlock);

			glGetActiveUniformBlockiv(mOGLProgramObject, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

			//glGetActiveUniformBlockiv(mOGLProgramObject, i, GL_UNIFORM_BLOCK_BINDING, &blockBinding);
			//glGetActiveUniformBlockiv(mOGLProgramObject, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &uniformIndices);

			indices.resize(numUniformInBlock);
			offset.resize(numUniformInBlock);
			types.resize(numUniformInBlock);
			arraySize.resize(numUniformInBlock);
			arrayStrides.resize(numUniformInBlock);
			matrixStrides.resize(numUniformInBlock);
			glGetUniformIndices(mOGLProgramObject, numUniformInBlock, &blockVariableNames[i][0], &indices[0]);
			glGetActiveUniformsiv(mOGLProgramObject, numUniformInBlock, &indices[0], GL_UNIFORM_OFFSET, &offset[0]);
			glGetActiveUniformsiv(mOGLProgramObject, numUniformInBlock, &indices[0], GL_UNIFORM_TYPE, &types[0]);
			glGetActiveUniformsiv(mOGLProgramObject, numUniformInBlock, &indices[0], GL_UNIFORM_SIZE, &arraySize[0]);
			glGetActiveUniformsiv(mOGLProgramObject, numUniformInBlock, &indices[0], GL_UNIFORM_ARRAY_STRIDE, &arrayStrides[0]);
			glGetActiveUniformsiv(mOGLProgramObject, numUniformInBlock, &indices[0], GL_UNIFORM_MATRIX_STRIDE, &matrixStrides[0]);

			GLuint blockIdx = glGetUniformBlockIndex(mOGLProgramObject, &name[0]);

			// Get or create constant buffer
			EffectConstantBuffer* cbuffer = mEffect.FetchConstantBuffer(String(&name[0], nameLen), blockSize);

			// Can specify binding slot in GLSL ?
			glUniformBlockBinding(mOGLProgramObject, blockIdx, i);
			AddConstantBufferBind(i, cbuffer);

			for (size_t j = 0; j < blockVariableNames[i].size(); ++j)
			{
				EffectParameter* parameter = blockVariables[i][j];
				cbuffer->AddEffectParameter(parameter, offset[j]);

				if (arrayStrides[j] > 0)
					parameter->SetArrayStride(arrayStrides[j]);

				if (matrixStrides[j] > 0)
					parameter->SetMatrixStride(matrixStrides[j]);	

				parameter->MakeDirty();
			}
		}
	}
	
	OGL_ERROR_CHECK();
}

void OpenGLShaderProgram::AddConstantBufferBind( GLuint bindingSlot, EffectConstantBuffer* effectCBuffer )
{
	mParameterBinds.push_back(ShaderParameterSetHelper<EffectConstantBuffer>(bindingSlot, effectCBuffer));
}

void OpenGLShaderProgram::AddParameterBind( GLint location, EffectParameter* effectParam, GLsizei arrSize )
{
	switch(effectParam->GetParameterType())
	{
	case EPT_Boolean:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<bool*>(location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<bool>(location, effectParam) );
		}
		break;
	case EPT_Int:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<int32_t*>(location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<int32_t>(location, effectParam) );
		}
		break;
	case EPT_Float:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float*>(location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float>(location, effectParam) );
		}
		break;
	case EPT_Float2:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float2*>(location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float2>(location, effectParam) );	
		}
		break;
	case EPT_Float3:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float3*>(location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float3>(location, effectParam) );	
		}
		break;
	case EPT_Float4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float4*>(location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float4>(location, effectParam) );		
		}
		break;
	case EPT_Matrix4x4:
		{
			if (arrSize > 1)
				mParameterBinds.push_back( ShaderParameterSetHelper<float4x4*>(location, effectParam, arrSize) );
			else 
				mParameterBinds.push_back( ShaderParameterSetHelper<float4x4>(location, effectParam) );		
		}
		break;
	case EPT_Texture1D:
	case EPT_Texture1DArray:
	case EPT_Texture2D:
	case EPT_Texture2DArray:
	case EPT_Texture3D:
	case EPT_Texture3DArray:
	case EPT_TextureCUBE:
	case EPT_TextureCUBEArray:
		{
			if (arrSize > 1)
			{
				assert(false);
			}
			else
			{
				mParameterBinds.push_back( ShaderParameterSetHelper<TextureLayer>(location, effectParam) );	
			}
		}
		break;
	default:
		assert(false);
	}
}

shared_ptr<ShaderProgram> OpenGLShaderProgram::Clone( Effect& effect )
{
	shared_ptr<OpenGLShaderProgram> retVal = std::make_shared<OpenGLShaderProgram>(effect);

	// copy attached shader
	retVal->mAttachedShaders.resize(mAttachedShaders.size());
	for (size_t i = 0; i < mAttachedShaders.size(); ++i)
	{
		// not create new shader object, because a shader object can be attached 
		// to more than one program object
		retVal->mAttachedShaders[i] = mAttachedShaders[i];
	}
	
	// link program
	retVal->LinkProgram();


	//retVal->mOGLProgramObject = glCreateProgram();
	//if (!retVal->mOGLProgramObject)
	//{
	//	retVal->mLinkerOutput = "Could not create shader program";
	//	return nullptr;
	//}

	//for (auto iter = retVal->mAttachedShaders.begin(); iter != retVal->mAttachedShaders.end(); ++iter)
	//{
	//	shared_ptr<OpenGLShader> shaderOGL = std::static_pointer_cast<OpenGLShader>(*iter);
	//	glAttachShader(retVal->mOGLProgramObject, shaderOGL->GetShaderObject());
	//}
	//glLinkProgram(retVal->mOGLProgramObject);

	//int linked;
	//glGetProgramiv(retVal->mOGLProgramObject, GL_LINK_STATUS, &linked);
	//retVal->mValidate = (linked != 0);
	//if (!retVal->mValidate)
	//{
	//	int length;
	//	glGetProgramiv(retVal->mOGLProgramObject, GL_INFO_LOG_LENGTH, &length);
	//	retVal->mLinkerOutput.resize(length);
	//	glGetProgramInfoLog(retVal->mOGLProgramObject, length, &length, &retVal->mLinkerOutput[0]);
	//}
	//else
	//{
	//	retVal->mLinkerOutput.clear();
	//}

	//if (!retVal->mValidate)
	//	return nullptr;

	//retVal->CaptureAllParameter();


	//printf("Clone shader program ID: %d \n", retVal->mOGLProgramObject);


	return retVal;
}

}