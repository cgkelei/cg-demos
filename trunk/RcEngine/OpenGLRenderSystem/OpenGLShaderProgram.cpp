#include "OpenGLShaderProgram.h"
#include "OpenGLShader.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Core/Exception.h>

namespace RcEngine {

template <typename T>
struct ShaderParameterSetHelper {};

template<>
struct ShaderParameterSetHelper<bool>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(Location), Param(param) {}

	void operator() ()
	{
		bool value; Param->GetValue(value);
		glUniform1i(Location, value);
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
		vector<bool> value; Param->GetValue(value);
		if (!value.empty())
		{
			vector<int32_t> tem(value.begin(), value.end());
			glUniform1iv(Location, tem.size(), &tem[0]);
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
		int32_t value; Param->GetValue(value);
		glUniform1i(Location, value);
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
		vector<int32_t> value; Param->GetValue(value);
		glUniform1iv(Location, value.size(), &value[0]);
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
		float value; Param->GetValue(value);
		glUniform1f(Location, value);
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
		vector<float> value; Param->GetValue(value);
		glUniform1fv(Location, value.size(), &value[0]);
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<Vector2f>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		Vector2f value; Param->GetValue(value);
		glUniform2f(Location, value.X(), value.Y());
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<Vector2f*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		vector<Vector2f> value; Param->GetValue(value);
		if (!value.empty())
		{
			glUniform2fv(Location, value.size(), 
				reinterpret_cast<float*>(&value[0][0]));
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<Vector3f>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		Vector3f value; Param->GetValue(value);
		glUniform3f(Location, value[0], value[1], value[2]);
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<Vector3f*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		vector<Vector3f> value; Param->GetValue(value);
		if (!value.empty())
		{
			glUniform3fv(Location, value.size(), 
				reinterpret_cast<float*>(&value[0][0]));
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<Vector4f>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		Vector4f value; Param->GetValue(value);
		glUniform4f(Location, value[0], value[1], value[2], value[3]);
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<Vector4f*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		vector<Vector4f> value; Param->GetValue(value);
		if (!value.empty())
		{
			glUniform3fv(Location, value.size(), 
				reinterpret_cast<float*>(&value[0][0]));
		}
	}

private:
	GLint Location;
	EffectParameter* Param;
};


template <>
struct ShaderParameterSetHelper<Matrix4f>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		Matrix4f value; Param->GetValue(value);
		glUniformMatrix4fv(Location, 1, false, &value[0]);
	}

private:
	GLint Location;
	EffectParameter* Param;
};

template <>
struct ShaderParameterSetHelper<Matrix4f*>
{
public:
	ShaderParameterSetHelper(GLint location, EffectParameter* param)
		: Location(location), Param(param) { }

	void operator() ()
	{
		vector<Matrix4f> value; Param->GetValue(value);
		if (!value.empty())
		{
			glUniformMatrix4fv(Location, value.size(), false,
				reinterpret_cast<float*>(&value[0][0]));
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

}

void OpenGLShaderProgram::Bind()
{
	glUseProgram(mOGLProgramObject);
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
		glDeleteProgram(mOGLProgramObject);
		mOGLProgramObject = 0;
	}

}

void OpenGLShaderProgram::CaptureAllParameter()
{
	// Query and store uniforms from the program.
	GLint activeUniforms;
	glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORMS, &activeUniforms);
	if (activeUniforms > 0)
	{
		mParameterBinds.resize(activeUniforms);

		GLint maxLength;
		glGetProgramiv(mOGLProgramObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
		vector<char> maxName(maxLength);

		bool isArray = false;

		GLint length;
		GLint uniformSize;
		GLenum uniformType;
		unsigned int samplerIndex = 0;	

		for (GLint i = 0; i < activeUniforms; ++i)
		{
			glGetActiveUniform(mOGLProgramObject, i, maxLength, &length, &uniformSize, &uniformType, &maxName[0]);

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

			mParameterBinds[i].Name = actualName;

			EffectParameterType effectParamType;
			OpenGLMapping::UnMapping(effectParamType, uniformType);

			
			EffectParameter* effectParam = mEffect.AddShaderParameterInternal(actualName, effectParamType, isArray);

			switch(effectParamType)
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
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Vector2f*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Vector2f>(location, effectParam);
					}	
				}
				break;
			case EPT_Float3:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Vector3f*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Vector3f>(location, effectParam);
					}	
				}
				break;
			case EPT_Float4:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Vector4f*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Vector4f>(location, effectParam);
					}	
				}
				break;
			case EPT_Matrix4x4:
				{
					if (isArray)
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Matrix4f*>(location, effectParam);
					}
					else
					{
						mParameterBinds[i].ShaderParamSetFunc = ShaderParameterSetHelper<Matrix4f>(location, effectParam);
					}	
				}
				break;
			case EPT_Texture2D:
				{
					if (isArray)
					{

					}
					else
					{

					}
				}
				break;
			default:
				assert(false);
			}

			mParameterBinds[i].Type = effectParamType;
		}
			//mParameterBinds[i].TextureSamplerIndex =  GL_SAMPLER_2D ? (samplerIndex++) : 0;
	}
}

}