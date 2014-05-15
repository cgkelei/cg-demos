#include "OpenGLShader.h"
#include "OpenGLDevice.h"
#include "OpenGLView.h"
#include "OpenGLBuffer.h"
#include <Graphics/EffectParameter.h>
#include <Graphics/Effect.h>
#include <Core/Exception.h>
#include <Core/Loger.h>

namespace RcEngine {

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
struct ShaderParameterSetHelper< EffectConstantBuffer >
{
public:
	ShaderParameterSetHelper(EffectConstantBuffer* param, GLuint bindingSlot)
		: BindingSlot(bindingSlot), UniformBlock(param) { }

	void operator() ()
	{
		// Update uniform buffer if changed
		UniformBlock->UpdateBuffer();

		GLuint bufferOGL = (static_cast_checked<OpenGLBuffer*>(UniformBlock->GetBuffer().get()))->GetBufferOGL();
		glBindBufferBase(GL_UNIFORM_BUFFER, BindingSlot, bufferOGL);
	}

private:
	GLuint BindingSlot;
	EffectConstantBuffer* UniformBlock;
};

template<>
struct ShaderParameterSetHelper<ShaderResourceView>
{
	ShaderParameterSetHelper(EffectParameter* param, GLuint binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			weak_ptr<ShaderResourceView> srv;
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
struct ShaderParameterSetHelper<UnorderedAccessView>
{
	ShaderParameterSetHelper(EffectParameter* param, GLuint binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		if (Param->GetTimeStamp() != UpdateTimeStamp)
		{
			weak_ptr<UnorderedAccessView> uav;
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

template<>
struct ShaderParameterSetHelper<SamplerState>
{
	ShaderParameterSetHelper(EffectParameter* param, GLuint binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		weak_ptr<SamplerState> sampler;
		Param->GetValue(sampler);

		if (auto spt = sampler.lock())
		{
			gOpenGLDevice->SetSamplerState(ST_Pixel /*Use used in OpenGL*/, Binding, spt);
		}
	}

private:
	GLuint Binding;
	EffectParameter* Param;
};

// Commit for set binding slot to shader program
struct ShaderResourceBinding
{
	enum BindType { UniformBlock, ShaderStorage, General };

	ShaderResourceBinding(BindType type, GLuint shaderID, GLint location, GLuint binding)
		: Type(type), ShaderOGL(shaderID), Location(location), Binding(binding) {}

	void operator() ()
	{
		switch (Type)
		{
		case UniformBlock:
			glUniformBlockBinding(ShaderOGL, Location, Binding);
			break;
		case ShaderStorage:
			glShaderStorageBlockBinding(ShaderOGL, Location, Binding);
			break;
		case General:
		default:
			glProgramUniform1i(ShaderOGL, Location, Binding);
			break;
		}
	}

private:
	BindType Type;

	GLuint ShaderOGL;
	GLuint Binding;
	GLuint Location;
};

//struct ShaderTextureBinding
//{
//	ShaderTextureBinding(GLuint shaderID, GLint location, GLuint binding)
//		: ShaderOGL(shaderID), Location(location), Binding(binding) {}
//
//	void operator() ()
//	{
//		glProgramUniform1i(ShaderOGL, Location, Binding);
//	}
//
//private:
//	GLuint ShaderOGL;
//	GLuint Binding;
//	GLuint Location;
//};
//
//struct ShaderStorageBinding
//{
//	ShaderStorageBinding(GLuint shaderID, GLint location, GLuint binding)
//		: ShaderOGL(shaderID), Location(location), Binding(binding) {}
//
//	void operator() ()
//	{
//		
//	}
//
//private:
//	GLuint ShaderOGL;
//	GLuint Binding;
//	GLuint Location;
//};

//////////////////////////////////////////////////////////////////////////
OpenGLShaderPipeline::OpenGLShaderPipeline(Effect& effect)
	: ShaderPipeline(effect)
{
	glGenProgramPipelines(1, &mPipelineOGL);
}

OpenGLShaderPipeline::~OpenGLShaderPipeline()
{
	glDeleteProgramPipelines(1, &mPipelineOGL);
}

void OpenGLShaderPipeline::OnBind()
{
	// Commit all shader parameter
	for (auto& paramBindFunc : mParameterBinds)
		paramBindFunc();
}

void OpenGLShaderPipeline::OnUnbind()
{
	//glBindProgramPipeline(0);
}

bool OpenGLShaderPipeline::LinkPipeline()
{
	GLuint srvBinding = 0;
	GLuint uavBinding = 0;
	GLuint uniformBlockBinding = 0;
	std::map<String, GLuint> mBindingCache;

	for (int i = 0; i < ST_Count; ++i)
	{
		if (mShaderStages[i])
		{
			OpenGLShader* shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[i].get()));

			// Uniform buffers
			for (const UniformBuffer& uniformBlock : shaderOGL->mUniformBuffers)
			{
				if (uniformBlock.Location == -1) // Global uniforms
				{
					for (const auto& globalUniform : uniformBlock.BufferVariables)
					{
						EffectParameter* effectParam = mEffect.FetchUniformParameter(globalUniform.Name, globalUniform.Type, globalUniform.ArraySize);
						AddUniformParamBind(shaderOGL->mShaderOGL, globalUniform.Location, effectParam, globalUniform.ArraySize);
					}
				}
				else
				{
					EffectConstantBuffer* uniformBuffer = mEffect.FetchConstantBuffer(uniformBlock.Name, uniformBlock.BufferSize);

					if (uniformBuffer->GetNumVariables() > 0) // Already created
					{
						// check buffer variables
						assert(uniformBuffer->GetNumVariables() == uniformBlock.BufferVariables.size());
						for (size_t i = 0; i < uniformBlock.BufferVariables.size(); ++i)
						{
							EffectParameter* variable = uniformBuffer->GetVariable(i);
							if (variable->GetName() != uniformBlock.BufferVariables[i].Name				||
								variable->GetParameterType() != uniformBlock.BufferVariables[i].Type		||
								variable->GetElementSize() != uniformBlock.BufferVariables[i].ArraySize ||
								variable->GetOffset() != uniformBlock.BufferVariables[i].Location)
							{
								ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Error: Same uniform buffer with different variables!", "D3D11ShaderPipeline::LinkPipeline");
							}
						}
					}
					else
					{
						for (const auto& bufferVariable : uniformBlock.BufferVariables)
						{
							EffectParameter* variable = mEffect.FetchUniformParameter(bufferVariable.Name, bufferVariable.Type, bufferVariable.ArraySize);
							uniformBuffer->AddVariable(variable, bufferVariable.Offset);

							if (bufferVariable.ArraySize > 1)
								variable->SetArrayStride(bufferVariable.ArrayStride);
						}
					}

					// Commit for binding uniform buffer to UBO binding slot 
					if (mBindingCache.find(uniformBlock.Name) == mBindingCache.end())
					{
						mBindingCache[uniformBlock.Name] = uniformBlockBinding++;
						AddUnitformBlockBind(uniformBuffer, mBindingCache[uniformBlock.Name]);
					}
					
					// Commit for set binding slot to shader context
					AddShaderResourceBind(shaderOGL->mShaderOGL, uniformBlock.Location, mBindingCache[uniformBlock.Name], ShaderResourceBinding::UniformBlock);
				}
			}

			// Shader resource views
			for (const ResouceViewParam& viewParam : shaderOGL->mBoundResources)
			{
				if (mBindingCache.find(viewParam.Name) == mBindingCache.end())
				{		
					// Bind resource to OpenGL binding slot, Texture/Image unit or SSBO
					if (viewParam.ViewClass == Shader_Param_SRV)
					{
						mBindingCache[viewParam.Name] = srvBinding++;

						EffectParameter* srvParam = mEffect.FetchSRVParameter(viewParam.Name, viewParam.Type);
						AddSRVResouceBind(srvParam, mBindingCache[viewParam.Name]);
					}
					else
					{
						mBindingCache[viewParam.Name] = uavBinding++;

						EffectParameter* uavParam = mEffect.FetchUAVParameter(viewParam.Name, viewParam.Type);
						AddUAVResourceBind(uavParam, mBindingCache[viewParam.Name]);
					}
				}

				// Set binding point to shader uniform
				if (viewParam.Type == EPT_StructureBuffer)
					AddShaderResourceBind(shaderOGL->mShaderOGL, viewParam.Location, mBindingCache[viewParam.Name], ShaderResourceBinding::ShaderStorage);
				else
					AddShaderResourceBind(shaderOGL->mShaderOGL, viewParam.Location, mBindingCache[viewParam.Name], ShaderResourceBinding::General);
			}

			// SamplerState
			for (auto& kv : shaderOGL->mSamplerStates)
			{
				EffectParameter* effectParam = mEffect.FetchSamplerParameter(kv.second);
				mParameterBinds.push_back( ShaderParameterSetHelper<SamplerState>(effectParam, mBindingCache[kv.first]) );
			}
		}
	}

	// Link all stage into pipeline
	if (mShaderStages[ST_Vertex])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Vertex].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_VERTEX_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_TessControl])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_TessControl].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_TESS_CONTROL_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_TessEval])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_TessEval].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_TESS_EVALUATION_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_Geomerty])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Geomerty].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_GEOMETRY_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_Pixel])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Pixel].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_FRAGMENT_SHADER_BIT, shaderOGL);
	}

	if (mShaderStages[ST_Compute])
	{
		GLuint shaderOGL = (static_cast<OpenGLShader*>(mShaderStages[ST_Compute].get()))->mShaderOGL;
		glUseProgramStages(mPipelineOGL, GL_COMPUTE_SHADER_BIT, shaderOGL);
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

void OpenGLShaderPipeline::AddUnitformBlockBind( EffectConstantBuffer* effectCBuffer, GLuint binding )
{
	mParameterBinds.push_back(ShaderParameterSetHelper<EffectConstantBuffer>(effectCBuffer, binding));
}

void OpenGLShaderPipeline::AddSRVResouceBind( EffectParameter* effectParam, GLuint binding )
{
	mParameterBinds.push_back( ShaderParameterSetHelper<ShaderResourceView>(effectParam, binding) );
}

void OpenGLShaderPipeline::AddUAVResourceBind( EffectParameter* effectParam, GLuint binding )
{
	mParameterBinds.push_back( ShaderParameterSetHelper<UnorderedAccessView>(effectParam, binding) );
}

void OpenGLShaderPipeline::AddShaderResourceBind( GLuint shader, GLint location, GLuint binding, GLuint bindType )
{
	mParameterBinds.push_back( ShaderResourceBinding(ShaderResourceBinding::BindType(bindType), shader, location, binding) );
}


}