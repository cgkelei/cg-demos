#include "OpenGLShaderProgram.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLSamplerState.h"
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
		: Location(location), Param(param) {}

	void operator() ()
	{
		if (Param->Dirty())
		{
			bool value; Param->GetValue(value);
			glUniform1ui(Location, value);
			Param->ClearDirty();
		} 
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template<>
struct ShaderParameterSetHelper<bool*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(Location), Param(param) {}

	void operator() ()
	{
		if (Param->Dirty())
		{
			vector<bool> value; Param->GetValue(value);
			if (!value.empty())
			{
				vector<int32_t> tem(value.begin(), value.end());
				glUniform1iv(Location, tem.size(), &tem[0]);
			}
			Param->ClearDirty();
		}

	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<int32_t>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			int32_t value; Param->GetValue(value);
			glUniform1i(Location, value);
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<int32_t*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			vector<int32_t> value; Param->GetValue(value);
			glUniform1iv(Location, value.size(), &value[0]);
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			float value; Param->GetValue(value);
			glUniform1f(Location, value);
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			vector<float> value; Param->GetValue(value);
			glUniform1fv(Location, value.size(), &value[0]);
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float2>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			float2 value; Param->GetValue(value);
			glUniform2f(Location, value.X(), value.Y());
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float2*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			vector<float2> value; Param->GetValue(value);
			if (!value.empty())
			{
				glUniform2fv(Location, value.size(), 
					reinterpret_cast<float*>(&value[0][0]));
			}
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float3>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			float3 value; Param->GetValue(value);
			glUniform3f(Location, value[0], value[1], value[2]);
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float3*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			vector<float3> value; Param->GetValue(value);
			if (!value.empty())
			{
				glUniform3fv(Location, value.size(), 
					reinterpret_cast<float*>(&value[0][0]));
			}
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			float4 value; Param->GetValue(value);
			glUniform4f(Location, value[0], value[1], value[2], value[3]);
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			vector<float4> value; Param->GetValue(value);
			if (!value.empty())
			{
				glUniform3fv(Location, value.size(), 
					reinterpret_cast<float*>(&value[0][0]));
			}
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};


template <>
struct ShaderParameterSetHelper<float4x4>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			float4x4 value; Param->GetValue(value);

			// we know that glsl matrix is column major, so we need transpose out matrix.
			glUniformMatrix4fv(Location, 1, true, &value[0]);
			Param->ClearDirty();
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<float4x4*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		if (Param->Dirty())
		{
			vector<float4x4> value; Param->GetValue(value);
			if (!value.empty())
			{
				glUniformMatrix4fv(Location, value.size(), true,
					reinterpret_cast<float*>(&value[0][0]));
			}
			Param->ClearDirty();
		}	
	}

private:
	GLint Location;
	EffectParameter* Param;
};

// note that a texture object always need a sample state to sample it 
template<>
struct ShaderParameterSetHelper< TextureLayer >
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		TextureLayer textureLayer; Param->GetValue(textureLayer);
		shared_ptr<OpenGLTexture> textureOGL = std::static_pointer_cast<OpenGLTexture>(textureLayer.Texture);

		if (textureOGL)
		{
			if (Param->Dirty())
			{	
				RenderDevice& device = Context::GetSingleton().GetRenderDevice();
				device.SetSamplerState(textureLayer.Stage, textureLayer.TexUnit, textureLayer.Sampler);

				Param->ClearDirty();
			}

			glActiveTexture(GL_TEXTURE0+textureLayer.TexUnit);
			glBindTexture(textureOGL->GetOpenGLTextureTarget(), textureOGL->GetOpenGLTexture());

			glUniform1i(Location, textureLayer.TexUnit);		
		}	
	}

private:
	GLint Location;
	EffectParameter* Param;
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

	for (ParameterBind& paramBind : mParameterBinds)
		paramBind.ShaderParamSetFunc();
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

OpenGLShaderProgram::ParameterBind OpenGLShaderProgram::GetShaderParamBindFunc(GLint location, EffectParameter* effectParam, bool isArray)
{
	ParameterBind paramBind;
	paramBind.EffectParameter = effectParam;
	paramBind.Location = location;
	paramBind.Name = effectParam->GetName();
	paramBind.Type = effectParam->GetParameterType();
	paramBind.IsArray = isArray;

	switch(paramBind.Type)
	{
	case EPT_Boolean:
		{
			if (isArray)
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<bool*>(location, effectParam);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<bool>(location, effectParam);
			}	
		}
		break;
	case EPT_Int:
		{
			if (isArray)
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<int32_t*>(location, effectParam);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<int32_t>(location, effectParam);
			}	
		}
		break;
	case EPT_Float:
		{
			if (isArray)
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float*>(location, effectParam);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float>(location, effectParam);
			}	
		}
		break;
	case EPT_Float2:
		{
			if (isArray)
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float2*>(location, effectParam);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float2>(location, effectParam);
			}	
		}
		break;
	case EPT_Float3:
		{
			if (isArray)
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float3*>(location, effectParam);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float3>(location, effectParam);
			}	
		}
		break;
	case EPT_Float4:
		{
			if (isArray)
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float4*>(location, effectParam);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float4>(location, effectParam);
			}	
		}
		break;
	case EPT_Matrix4x4:
		{
			if (isArray)
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float4x4*>(location, effectParam);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<float4x4>(location, effectParam);
			}	
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
			if (isArray)
			{
				assert(false);
			}
			else
			{
				paramBind.ShaderParamSetFunc = ShaderParameterSetHelper<TextureLayer>(location, effectParam);
			}
		}
		break;
	default:
		assert(false);
	}

	return paramBind;
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
	GLint activeAttribs;
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

		/*	for (GLint i = 0; i < activeAttribs; ++i)
		{
		glGetActiveAttrib(mOGLProgramObject, i, maxLength, &length, &attibSize, &attibType, &maxName[0]);
		String actualName(&maxName[0], length);
		auto attrPos = glGetAttribLocation(mOGLProgramObject, actualName.c_str());

			std::cout << actualName << " attr=" << attrPos << " size=" << attibSize << " type=" << GetGLSLTypeString(attibType) << std::endl;
		}
		std::cout << std::endl;*/

		/*	auto iPos = glGetAttribLocation(mOGLProgramObject, "iPos");
		auto iBlendWeights = glGetAttribLocation(mOGLProgramObject, "iBlendWeights");
		auto iBlendIndices = glGetAttribLocation(mOGLProgramObject, "iBlendIndices");
		auto iNormal = glGetAttribLocation(mOGLProgramObject, "iNormal");
		auto iTex = glGetAttribLocation(mOGLProgramObject, "iTex");
		auto iTangent = glGetAttribLocation(mOGLProgramObject, "iTangent");
		auto iBinormal = glGetAttribLocation(mOGLProgramObject, "iBinormal");*/
	}

	// Query and store uniforms from the program.
	GLint activeUniforms;
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORMS, &activeUniforms);
	if (activeUniforms > 0)
	{
		mParameterBinds.resize(activeUniforms);

		GLint maxLength;
		glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
		vector<char> maxName(maxLength);

		GLint length;
		GLint uniformSize;
		GLenum uniformType;
		unsigned int samplerIndex = 0;	

		for (GLint i = 0; i < activeUniforms; ++i)
		{
			glGetActiveUniform(mOGLProgramObject, i, maxLength, &length, &uniformSize, &uniformType, &maxName[0]);

			bool isArray = false;
			if (uniformSize > 1 && maxLength > 3)
			{
				// This is an array uniform
				isArray = true;
				GLint lengthWithoutBracket = 0;
				while( lengthWithoutBracket < length && maxName[lengthWithoutBracket] != '[') lengthWithoutBracket++;
				length = lengthWithoutBracket;
			}

			String actualName(&maxName[0], length);
			GLint location = glGetUniformLocation(mOGLProgramObject, actualName.c_str());

			EffectParameterType effectParamType;
			OpenGLMapping::UnMapping(effectParamType, uniformType);

			//printf("\t%s\n", actualName.c_str());
			//std::cout << actualName << " size=" << uniformSize << " type=" << GetGLSLTypeString(uniformType) << std::endl;

			EffectParameter* effectParam = mEffect.FetchShaderParameter(actualName, effectParamType, isArray);

			mParameterBinds[i] = GetShaderParamBindFunc(location, effectParam, isArray);


		/*	switch(effectParamType)
			{
			case EPT_Boolean:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<bool*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<bool>(location, effectParam);
					}	
				}
				break;
			case EPT_Int:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<int32_t*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<int32_t>(location, effectParam);
					}	
				}
				break;
			case EPT_Float:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float>(location, effectParam);
					}	
				}
				break;
			case EPT_Float2:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float2*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float2>(location, effectParam);
					}	
				}
				break;
			case EPT_Float3:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float3*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float3>(location, effectParam);
					}	
				}
				break;
			case EPT_Float4:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float4*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float4>(location, effectParam);
					}	
				}
				break;
			case EPT_Matrix4x4:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float4x4*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<float4x4>(location, effectParam);
					}	
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
					if (isArray)
					{
						assert(false);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<TextureLayer>(location, effectParam);
					}
				}
				break;
			default:
				assert(false);
			}

			mParameterBinds[i].Type = effectParamType;*/
		}
			//mParameterBinds[i].TextureSamplerIndex =  GL_SAMPLER_2D ? (samplerIndex++) : 0;
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