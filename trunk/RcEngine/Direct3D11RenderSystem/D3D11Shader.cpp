#include "D3D11Shader.h"
#include "D3D11Device.h"
#include "D3D11State.h"
#include "D3D11Buffer.h"
#include "D3D11View.h"
#include "D3D11GraphicCommon.h"
#include <Graphics/EffectParameter.h>
#include <Graphics/Effect.h>
#include <Core/Loger.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <fstream>

namespace RcEngine {

struct SRVBindHelper
{
	SRVBindHelper(EffectParameter* param, uint32_t binding, ShaderType shaderStage)
		: Param(param), Binding(binding), ShaderStage(shaderStage) {}

	void operator() ()
	{
		weak_ptr<ShaderResourceView> srv;
		Param->GetValue(srv);

		if (auto spt = srv.lock())
		{
			ID3D11ShaderResourceView* srvD3D11 = static_cast_checked<D3D11ShaderResouceView*>(spt.get())->ShaderResourceViewD3D11;;
			
			ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;

			switch (ShaderStage)
			{
			case ST_Vertex:
				deviceContext->VSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Hull:
				deviceContext->HSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Domain:
				deviceContext->DSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Geomerty:
				deviceContext->GSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Pixel:
				deviceContext->PSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			case ST_Compute:
				deviceContext->CSSetShaderResources(Binding, 1, &srvD3D11);
				break;
			default:
				break;
			}
		}
	}

private:
	EffectParameter* Param;
	uint32_t Binding;
	ShaderType ShaderStage;
};

struct UAVSRVBindHelper
{
	UAVSRVBindHelper(EffectParameter* param, uint32_t binding)
		: Param(param), Binding(binding) {}

	void operator() ()
	{
		weak_ptr<UnorderedAccessView> uav;
		Param->GetValue(uav);

		if (auto spt = uav.lock())
		{
			ID3D11UnorderedAccessView* uavD3D11 = static_cast_checked<D3D11UnorderedAccessView*>(spt.get())->UnorderedAccessViewD3D11;;

			ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;
			deviceContext->CSSetUnorderedAccessViews(Binding, 1, &uavD3D11, nullptr);
		}
	}

private:
	EffectParameter* Param;
	uint32_t Binding;
};

struct SamplerBindHelper
{
	SamplerBindHelper(EffectParameter* param, uint32_t binding, ShaderType shaderStage)
		: Param(param), Binding(binding), ShaderStage(shaderStage) {}

	void operator() ()
	{
		weak_ptr<SamplerState> sampler;
		Param->GetValue(sampler);

		if (auto spt = sampler.lock())
		{
			ID3D11SamplerState* samplerStateD3D11 = static_cast_checked<D3D11SamplerState*>(spt.get())->StateD3D11;;
			
			ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;

			switch (ShaderStage)
			{
			case ST_Vertex:
				deviceContext->VSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Hull:
				deviceContext->HSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Domain:
				deviceContext->DSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Geomerty:
				deviceContext->GSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Pixel:
				deviceContext->PSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			case ST_Compute:
				deviceContext->CSSetSamplers(Binding, 1, &samplerStateD3D11);
				break;
			default:
				break;
			}
		}
	}

private:
	EffectParameter* Param;
	uint32_t Binding;
	ShaderType ShaderStage;
};

struct UniformBindHelper
{
	UniformBindHelper(EffectConstantBuffer* buffer, uint32_t binding, ShaderType shaderStage)
		: Buffer(buffer), Binding(binding), ShaderStage(shaderStage) {}

	void operator() ()
	{
		// Update uniform buffer if changed
		Buffer->UpdateBuffer();

		ID3D11Buffer* bufferD3D11 = static_cast_checked<D3D11Buffer*>(Buffer->GetBuffer().get())->BufferD3D11;

		ID3D11DeviceContext* deviceContext = gD3D11Device->DeviceContextD3D11;
		switch (ShaderStage)
		{
		case ST_Vertex:
			deviceContext->VSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Hull:
			deviceContext->HSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Domain:
			deviceContext->DSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Geomerty:
			deviceContext->GSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Pixel:
			deviceContext->PSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		case ST_Compute:
			deviceContext->CSSetConstantBuffers(Binding, 1, &bufferD3D11);
			break;
		default:
			break;
		}
	}

private:
	EffectConstantBuffer* Buffer;
	uint32_t Binding;
	ShaderType ShaderStage;
};

class D3D11ShaderReflection
{
public:
	D3D11ShaderReflection(D3D11Shader* shaderD3D11, void* bytecode, uint32_t size)
		: mShaderD3D11(shaderD3D11),
		  mReflectorD3D11(nullptr)
	{
		HRESULT hr;
		hr = D3DReflect( bytecode, size, IID_ID3D11ShaderReflection,  (void**) &mReflectorD3D11); 
		mReflectorD3D11->GetDesc( &mShaderDescD3D11 );
	}
	
	~D3D11ShaderReflection()
	{
		SAFE_RELEASE(mReflectorD3D11);
	}

	void ReflectShader()
	{
		if (mShaderD3D11->GetShaderType() == ST_Vertex)
			RefectInputParameters();

		ReflectConstantBuffers();
		RefectionBoundResources();
	}

	void RefectInputParameters()
	{
		D3D11VertexShader* vertexShaderD3D11 = static_cast_checked<D3D11VertexShader*>(mShaderD3D11);
		for ( UINT i = 0; i < mShaderDescD3D11.InputParameters; i++ ) 
		{ 
			D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc; 
			mReflectorD3D11->GetInputParameterDesc( i, &inputParamDesc ); 

			if (inputParamDesc.SystemValueType == D3D10_NAME_UNDEFINED)
			{
				InputSignature signature;
				signature.Semantic = inputParamDesc.SemanticName;
				signature.SemanticIndex = inputParamDesc.SemanticIndex;

				vertexShaderD3D11->InputSignatures.push_back(signature);
			}
			else
			{
				// System semantic, do not add to vertex input signature
			}
		} 
	}

	void ReflectConstantBuffers()
	{
		mShaderD3D11->ConstantBufferParams.resize(mShaderDescD3D11.ConstantBuffers);
		for ( UINT i = 0; i < mShaderDescD3D11.ConstantBuffers; i++ ) 
		{
			ID3D11ShaderReflectionConstantBuffer* bufferD3D11 = mReflectorD3D11->GetConstantBufferByIndex( i ); 

			D3D11_SHADER_BUFFER_DESC bufferDesc;
			bufferD3D11->GetDesc(&bufferDesc);
			
			ConstantBuffer& cbufferParam = mShaderD3D11->ConstantBufferParams[i];

			cbufferParam.Name = bufferDesc.Name;
			cbufferParam.BufferSize = bufferDesc.Size; 
			cbufferParam.Binding = i;

			cbufferParam.BufferVariables.resize(bufferDesc.Variables);
			for ( UINT j = 0; j < bufferDesc.Variables; j++ ) 
			{
				ID3D11ShaderReflectionVariable* variableD3D11 = bufferD3D11->GetVariableByIndex( j ); 
				ID3D11ShaderReflectionType* varTypeD3D11 = variableD3D11->GetType(); 

				D3D11_SHADER_VARIABLE_DESC varDesc; 
				D3D11_SHADER_TYPE_DESC varTypeDesc; 

				variableD3D11->GetDesc( &varDesc ); 
				varTypeD3D11->GetDesc( &varTypeDesc ); 

				UniformParam& param = cbufferParam.BufferVariables[j];

				param.Name = varDesc.Name;
				param.Offset = varDesc.StartOffset;
				param.ArraySize = varTypeDesc.Elements;

				switch (varTypeDesc.Class)
				{
				case D3D10_SVC_SCALAR:
					{
						switch (varTypeDesc.Type)
						{
						case D3D10_SVT_BOOL:	param.Type = EPT_Boolean; break;
						case D3D10_SVT_INT:		param.Type = EPT_Int; break;
						case D3D10_SVT_UINT:	param.Type = EPT_UInt; break;
						case D3D10_SVT_FLOAT:	param.Type = EPT_Float; break;
						default:
							break;
						}
					}
					break;
				case D3D10_SVC_VECTOR:
					{
						switch (varTypeDesc.Type)
						{
						case D3D10_SVT_BOOL:	param.Type = EPT_Boolean; break;
						case D3D10_SVT_INT:		param.Type = EffectParameterType(EPT_Int + varTypeDesc.Columns); break;
						case D3D10_SVT_UINT:	param.Type = EffectParameterType(EPT_UInt + varTypeDesc.Columns);; break;
						case D3D10_SVT_FLOAT:	param.Type = EffectParameterType(EPT_Float + varTypeDesc.Columns);; break;
						default:
							break;
						}
					}
					break;
				case D3D10_SVC_MATRIX_ROWS:
				case D3D10_SVC_MATRIX_COLUMNS:
					{
						if (varTypeDesc.Rows == 4 && varTypeDesc.Columns == 4)
							param.Type = EPT_Matrix4x4;
						else if (varTypeDesc.Rows == 3 && varTypeDesc.Columns == 3)
							param.Type = EPT_Matrix3x3;
						else if (varTypeDesc.Rows == 2 && varTypeDesc.Columns == 2)
							param.Type = EPT_Matrix2x2;
						else 
							assert(false);
					}
					break;
				case D3D10_SVC_STRUCT:
					{
						assert(false);
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void RefectionBoundResources()
	{
		for ( UINT i = 0; i < mShaderDescD3D11.BoundResources; i++ ) 
		{
			D3D11_SHADER_INPUT_BIND_DESC resBindDesc;
			mReflectorD3D11->GetResourceBindingDesc(i, &resBindDesc);
			
			switch (resBindDesc.Type)
			{
			case D3D10_SIT_CBUFFER:
				{
					printf("CBuffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D10_SIT_SAMPLER:
				{
					bool bCompare = (resBindDesc.uFlags & D3D10_SIF_COMPARISON_SAMPLER);
					//printf("Sampler: %s cmp=%d binding=%d\n", resBindDesc.Name, bCompare, resBindDesc.BindPoint);

					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_Sampler;
					param.Class = Shader_Param_Sampler;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D10_SIT_TEXTURE:  // Missing TextureUAV
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Class = Shader_Param_SRV;
					D3D11Mapping::UnMapping(resBindDesc.Dimension, param.Type);
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D10_SIT_TBUFFER:
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_TextureBuffer;
					param.Class = Shader_Param_SRV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D11_SIT_UAV_RWTYPED:
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_TextureBuffer;
					param.Class = Shader_Param_UAV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D11_SIT_STRUCTURED:
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_StructureBuffer;
					param.Class = Shader_Param_SRV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;
			case D3D11_SIT_UAV_RWSTRUCTURED:
				{
					ResourceInputParam param;
					param.Name = resBindDesc.Name;
					param.Binding = resBindDesc.BindPoint;
					param.Type = EPT_StructureBuffer;
					param.Class = Shader_Param_UAV;
					mShaderD3D11->ResourceInputParams.push_back(param);
				}
				break;

			case D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				{
					assert(false);
					printf("UAV Atomic StructuredBuffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D11_SIT_BYTEADDRESS:
			case D3D11_SIT_UAV_RWBYTEADDRESS:
			case D3D11_SIT_UAV_APPEND_STRUCTURED:
			case D3D11_SIT_UAV_CONSUME_STRUCTURED:
			default:
				assert(false);
				break;
			}

		}
	}

protected:
	D3D11Shader* mShaderD3D11;

	D3D11_SHADER_DESC mShaderDescD3D11; 
	ID3D11ShaderReflection* mReflectorD3D11;
};


bool LoadBinary(const char* filename, std::vector<uint8_t>& byteCode)
{
	FILE* fp = fopen(filename, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		long len = ftell(fp);
		byteCode.resize(len);

		fseek(fp, 0, SEEK_SET);
		fread(&byteCode[0], len, 1, fp);
		fclose(fp);

		return true;
	}
	return false;
}

// Helper function to dynamic compile HLSL shader code
HRESULT CompileHLSL(const String& filename, const ShaderMacro* macros, uint32_t macroCount,
	const String& entryPoint, const String& shaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;

	D3D10_SHADER_MACRO* pMacro = nullptr;
	std::vector<D3D10_SHADER_MACRO> d3dMacro;
	if (macros)
	{
		d3dMacro.resize(macroCount+1);
		for (uint32_t i = 0; i < macroCount; ++i)
		{
			d3dMacro[i].Name = macros[i].Name.c_str();
			d3dMacro[i].Definition = macros[i].Definition.c_str();
		}
		d3dMacro.back().Name = d3dMacro.back().Definition = nullptr;
		pMacro = &d3dMacro[0];
	}

	hr = D3DCompileFromFile(StringToWString(filename).c_str(), pMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), 
		shaderModel.c_str(), dwShaderFlags, 0,  ppBlobOut, &pErrorBlob);

	if( FAILED(hr) )
	{
		if (pErrorBlob)
		{
			const char* msg = (char*)( pErrorBlob->GetBufferPointer() );

			pErrorBlob->Release();
		}
		else
			EngineLogger::LogError("HLSL shader compile failed!");
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////

D3D11VertexShader::D3D11VertexShader()
	: D3D11Shader(ST_Vertex),
	  ShaderD3D11(nullptr)
{

}

D3D11VertexShader::~D3D11VertexShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11VertexShader::LoadFromByteCode( const String& filename )
{
	if ( LoadBinary(filename.c_str(), ShaderCode) == false )
	{
		ShaderCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}
	

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateVertexShader(&ShaderCode[0], ShaderCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Vertex Shader %s compile failed\n", filename.c_str());
		return false;
	}
	
	return true;
}

bool D3D11VertexShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "vs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		ShaderCode.resize(shaderBlob->GetBufferSize());
		std::memcpy(&ShaderCode[0], shaderBlob->GetBufferPointer(), ShaderCode.size());

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11HullShader::D3D11HullShader()
	: D3D11Shader(ST_Hull),
	  ShaderD3D11(nullptr)
{

}

D3D11HullShader::~D3D11HullShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11HullShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateHullShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11HullShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "hs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11DomainShader::D3D11DomainShader()
	: D3D11Shader(ST_Domain),
	  ShaderD3D11(nullptr)
{

}

D3D11DomainShader::~D3D11DomainShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11DomainShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateDomainShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11DomainShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "ds_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11GeometryShader::D3D11GeometryShader()
	: D3D11Shader(ST_Geomerty),
	  ShaderD3D11(nullptr)
{

}

D3D11GeometryShader::~D3D11GeometryShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11GeometryShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateGeometryShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11GeometryShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "gs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11PixelShader::D3D11PixelShader()
	: D3D11Shader(ST_Pixel),
	  ShaderD3D11(nullptr)
{

}

D3D11PixelShader::~D3D11PixelShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11PixelShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreatePixelShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11PixelShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "ps_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);
	
		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11ComputeShader::D3D11ComputeShader()
	: D3D11Shader(ST_Compute),
	  ShaderD3D11(nullptr)
{

}

D3D11ComputeShader::~D3D11ComputeShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11ComputeShader::LoadFromByteCode( const String& filename )
{
	std::vector<uint8_t> byteCode;
	if ( LoadBinary(filename.c_str(), byteCode) == false )
	{
		byteCode.clear();
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not exits!", "OpenGLShader::LoadFromByteCode");
		return false;
	}

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateComputeShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLogger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11ComputeShader::LoadFromFile( const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, macroCount, entryPoint, "cs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
		hr = deviceD3D11->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);

		D3D11ShaderReflection shaderReflection(this, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());	
		shaderReflection.ReflectShader();
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11ShaderPipeline::D3D11ShaderPipeline(Effect& effect)
	: ShaderPipeline(effect)
{

}

bool D3D11ShaderPipeline::LinkPipeline()
{
	for (int i = 0; i < ST_Count; ++i)
	{
		if (mShaderStages[i])
		{
			D3D11Shader* shaderD3D11 = static_cast_checked<D3D11Shader*>(mShaderStages[i].get());

			// Constant Buffer
			for (const auto& cbufferParam : shaderD3D11->ConstantBufferParams)
			{
				EffectConstantBuffer* constantBuffer;

				// Fetch a constant buffer if not exits
				if (cbufferParam.Name == "$Globals")
				{
					bool found = false;

					// Check to see if exits a same global uniform buffer
					uint32_t numBuffers = mEffect.GetNumConstantBuffers();	
					for (uint32_t j = 0; j < numBuffers && !found; ++j)
					{
						constantBuffer = mEffect.GetConstantBuffer(j);
						found = (constantBuffer->GetName() == "$Globals" && 
							     constantBuffer->GetBufferSize() == cbufferParam.BufferSize &&
							     constantBuffer->GetNumVariables() == cbufferParam.BufferVariables.size());

						for (size_t k = 0; found && (k < cbufferParam.BufferVariables.size()); ++k)
						{
							EffectParameter* variable = constantBuffer->GetVariable(k);
							// Check every buffer variable
							found &= (variable->GetName() == cbufferParam.BufferVariables[k].Name			    ||
									  variable->GetParameterType() == cbufferParam.BufferVariables[k].Type	    ||
								      variable->GetElementSize() == cbufferParam.BufferVariables[k].ArraySize   ||
								      variable->GetOffset() == cbufferParam.BufferVariables[k].Offset);
						}
					}

					if (!found)
					{
						// Create a new global CBuffer
						constantBuffer = mEffect.CreateConstantBuffer(cbufferParam.Name, cbufferParam.BufferSize);
					}
				}
				else
				{
					constantBuffer = mEffect.FetchConstantBuffer(cbufferParam.Name, cbufferParam.BufferSize);
					if (constantBuffer->GetNumVariables() > 0)
					{
						assert(constantBuffer->GetNumVariables() == cbufferParam.BufferVariables.size());
						for (size_t i = 0; i < cbufferParam.BufferVariables.size(); ++i)
						{
							EffectParameter* variable = constantBuffer->GetVariable(i);
							if (variable->GetName() != cbufferParam.BufferVariables[i].Name ||
								variable->GetParameterType() != cbufferParam.BufferVariables[i].Type ||
								variable->GetElementSize() != cbufferParam.BufferVariables[i].ArraySize ||
								variable->GetOffset() != cbufferParam.BufferVariables[i].Offset)
							{
								ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Error: Same uniform buffer with different variables!", "D3D11ShaderPipeline::LinkPipeline");
							}
						}
					}
				}
				
				if (constantBuffer->GetNumVariables() == 0)
				{
					for (const auto& bufferVariable : cbufferParam.BufferVariables)
					{
						EffectParameter* variable = mEffect.FetchUniformParameter(bufferVariable.Name, bufferVariable.Type, bufferVariable.ArraySize);
						constantBuffer->AddVariable(variable, bufferVariable.Offset);

						if (bufferVariable.ArraySize > 1)
							variable->SetArrayStride(sizeof(float) * 4); // always float4
					}
				}
			}

			for (const auto& param : shaderD3D11->ResourceInputParams)
			{
				switch (param.Class)
				{
				case Shader_Param_SRV:
					{
						EffectParameter* effectParam = mEffect.FetchSRVParameter(param.Name, param.Type);
						mParameterBinds.push_back( SRVBindHelper(effectParam, param.Binding, shaderD3D11->GetShaderType()) );
					}
					break;
				case Shader_Param_UAV:
					{
						// Only Compute has UAV
						if (mShaderStages[i]->GetShaderType() == ST_Compute)
						{
							EffectParameter* effectParam = mEffect.FetchSRVParameter(param.Name, param.Type);
							mParameterBinds.push_back( UAVSRVBindHelper(effectParam, param.Binding) );
						}
					}
					break;
				case Shader_Param_Sampler:
					{
						EffectParameter* effectParam = mEffect.FetchSamplerParameter(param.Name);
						mParameterBinds.push_back( SamplerBindHelper(effectParam, param.Binding, shaderD3D11->GetShaderType()) );
					}
					break;
				default:
					break;
				}
			}
		}
	}

	return true;
}

void D3D11ShaderPipeline::OnBind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	if (mShaderStages[ST_Vertex])
	{
		ID3D11VertexShader* shaderD3D11 = (static_cast<D3D11VertexShader*>(mShaderStages[ST_Vertex].get()))->ShaderD3D11;
		deviceContextD3D11->VSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStages[ST_Hull])
	{
		ID3D11HullShader* shaderD3D11 = (static_cast<D3D11HullShader*>(mShaderStages[ST_Hull].get()))->ShaderD3D11;
		deviceContextD3D11->HSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStages[ST_Domain])
	{
		ID3D11DomainShader* shaderD3D11 = (static_cast<D3D11DomainShader*>(mShaderStages[ST_Domain].get()))->ShaderD3D11;
		deviceContextD3D11->DSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStages[ST_Geomerty])
	{
		ID3D11GeometryShader* shaderD3D11 = (static_cast<D3D11GeometryShader*>(mShaderStages[ST_Geomerty].get()))->ShaderD3D11;
		deviceContextD3D11->GSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStages[ST_Pixel])
	{
		ID3D11PixelShader* shaderD3D11 = (static_cast<D3D11PixelShader*>(mShaderStages[ST_Pixel].get()))->ShaderD3D11;
		deviceContextD3D11->PSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStages[ST_Compute])
	{
		ID3D11ComputeShader* shaderD3D11 = (static_cast<D3D11ComputeShader*>(mShaderStages[ST_Compute].get()))->ShaderD3D11;
		deviceContextD3D11->CSSetShader(shaderD3D11, nullptr, 0);

	}

	// Commit all shader resource
	for ( auto& paramBindCommit : mParameterBinds) 
		paramBindCommit();
}

void D3D11ShaderPipeline::OnUnbind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	if (mShaderStages[ST_Vertex])   deviceContextD3D11->VSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Hull])     deviceContextD3D11->HSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Domain])   deviceContextD3D11->DSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Geomerty]) deviceContextD3D11->GSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Pixel])    deviceContextD3D11->PSSetShader(nullptr, nullptr, 0);
	if (mShaderStages[ST_Compute])  deviceContextD3D11->CSSetShader(nullptr, nullptr, 0);
}


}