#include "D3D11Shader.h"
#include "D3D11Device.h"
#include "D3D11GraphicCommon.h"
#include <Core/Loger.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <fstream>

namespace {

using namespace RcEngine;

class ShaderReflection
{
public:
	ShaderReflection(ShaderType shaderType, void* bytecode, uint32_t size)
		: mShaderType(shaderType),
		  mReflectorD3D11(nullptr)
	{
		HRESULT hr;
		hr = D3DReflect( bytecode, size, IID_ID3D11ShaderReflection,  (void**) &mReflectorD3D11); 
		mReflectorD3D11->GetDesc( &mShaderDescD3D11 );

		RefectInputParameters();
		ReflectOutputParameters();
		RefectionBoundResources();
	}
	
	~ShaderReflection()
	{
		SAFE_RELEASE(mReflectorD3D11);
	}

	void RefectInputParameters()
	{
		for ( UINT i = 0; i < mShaderDescD3D11.InputParameters; i++ ) 
		{ 
			D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc; 
			mReflectorD3D11->GetInputParameterDesc( i, &inputParamDesc ); 

			printf("Input Semantic: %s%d\n", inputParamDesc.SemanticName, inputParamDesc.SemanticIndex);
		} 
		
	}

	void ReflectOutputParameters()
	{
		for ( UINT i = 0; i < mShaderDescD3D11.OutputParameters; i++ ) 
		{ 
			D3D11_SIGNATURE_PARAMETER_DESC outputParamDesc; 
			mReflectorD3D11->GetInputParameterDesc( i, &outputParamDesc ); 
		} 
	}

	void ReflectConstantBuffers()
	{
		for ( UINT i = 0; i < mShaderDescD3D11.ConstantBuffers; i++ ) 
		{
			ID3D11ShaderReflectionConstantBuffer* bufferD3D11 = mReflectorD3D11->GetConstantBufferByIndex( i ); 

			D3D11_SHADER_BUFFER_DESC bufferDesc;
			bufferD3D11->GetDesc(&bufferDesc);
			
			String bufferName = bufferDesc.Name;
			uint32_t bufferSize = bufferDesc.Size; 

			for ( UINT j = 0; j < bufferDesc.Variables; j++ ) 
			{
				// Get the variable description and store it 
				ID3D11ShaderReflectionVariable* variableD3D11 = bufferD3D11->GetVariableByIndex( j ); 
				
				D3D11_SHADER_VARIABLE_DESC varDesc; 
				variableD3D11->GetDesc( &varDesc ); 

				// Get the variable type description and store it 
				ID3D11ShaderReflectionType* varTypeD3D11 = variableD3D11->GetType(); 
				
				D3D11_SHADER_TYPE_DESC varTypeDesc; 
				varTypeD3D11->GetDesc( &varTypeDesc ); 

	
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
					printf("Sampler: %s cmp=%d binding=%d\n", resBindDesc.Name, bCompare, resBindDesc.BindPoint);
				}
				break;
			case D3D10_SIT_TEXTURE:
				{
					TextureType texType;
					bool bTexArray;
					D3D11Mapping::UnMapping(resBindDesc.Dimension, texType, bTexArray);

					printf("Texture: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D10_SIT_TBUFFER:
				{
					printf("TBuffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D11_SIT_UAV_RWTYPED:
				{
					printf("UAV Buffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D11_SIT_STRUCTURED:
				{
					printf("StructuredBuffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D11_SIT_UAV_RWSTRUCTURED:
				{
					printf("UAV StructuredBuffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				{
					printf("UAV Atomic StructuredBuffer: %s binding=%d\n", resBindDesc.Name, resBindDesc.BindPoint);
				}
				break;
			case D3D11_SIT_BYTEADDRESS:
			case D3D11_SIT_UAV_RWBYTEADDRESS:
			case D3D11_SIT_UAV_APPEND_STRUCTURED:
			case D3D11_SIT_UAV_CONSUME_STRUCTURED:
				{

				}
				break;
			default:
				break;
			}

		}
	}

protected:
	ShaderType mShaderType;

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
		d3dMacro.resize(macroCount);
		for (uint32_t i = 0; i < macroCount; ++i)
		{
			d3dMacro[i].Name = macros[i].Name.c_str();
			d3dMacro[i].Definition = macros[i].Definition.c_str();
		}

		pMacro = &d3dMacro[0];
	}

	// Convert to wchar
	WString wfilename = StringToWString(filename);

	hr = D3DCompileFromFile(wfilename.c_str(), pMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), 
		shaderModel.c_str(), dwShaderFlags, 0,  ppBlobOut, &pErrorBlob);

	if( FAILED(hr) )
	{
		if (pErrorBlob)
		{
			const char* msg = (char*)( pErrorBlob->GetBufferPointer() );

			pErrorBlob->Release();
		}
		else
			EngineLoger::LogError("HLSL shader compile failed!");
	}

	return hr;
}



}

namespace RcEngine {

D3D11VertexShader::D3D11VertexShader()
	: RHShader(ST_Vertex),
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
		EngineLoger::LogError("HLSL Vertex Shader %s compile failed\n", filename.c_str());
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
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11HullShader::D3D11HullShader()
	: RHShader(ST_Hull),
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
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
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
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11DomainShader::D3D11DomainShader()
	: RHShader(ST_Domain),
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
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
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
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11GeometryShader::D3D11GeometryShader()
	: RHShader(ST_Geomerty),
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
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
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
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11PixelShader::D3D11PixelShader()
	: RHShader(ST_Pixel),
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
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
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
	
		ShaderReflection shaderReflection(ST_Pixel, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());		
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11ComputeShader::D3D11ComputeShader()
	: RHShader(ST_Compute),
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
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
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
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
D3D11ShaderProgram::D3D11ShaderProgram()
{

}

void D3D11ShaderProgram::Bind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	if (mShaderStage[ST_Vertex])
	{
		ID3D11VertexShader* shaderD3D11 = (static_cast<D3D11VertexShader*>(mShaderStage[ST_Vertex].get()))->ShaderD3D11;
		deviceContextD3D11->VSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStage[ST_Hull])
	{
		ID3D11HullShader* shaderD3D11 = (static_cast<D3D11HullShader*>(mShaderStage[ST_Hull].get()))->ShaderD3D11;
		deviceContextD3D11->HSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStage[ST_Domain])
	{
		ID3D11DomainShader* shaderD3D11 = (static_cast<D3D11DomainShader*>(mShaderStage[ST_Domain].get()))->ShaderD3D11;
		deviceContextD3D11->DSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStage[ST_Geomerty])
	{
		ID3D11GeometryShader* shaderD3D11 = (static_cast<D3D11GeometryShader*>(mShaderStage[ST_Geomerty].get()))->ShaderD3D11;
		deviceContextD3D11->GSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStage[ST_Pixel])
	{
		ID3D11PixelShader* shaderD3D11 = (static_cast<D3D11PixelShader*>(mShaderStage[ST_Pixel].get()))->ShaderD3D11;
		deviceContextD3D11->PSSetShader(shaderD3D11, nullptr, 0);
	}

	if (mShaderStage[ST_Compute])
	{
		ID3D11ComputeShader* shaderD3D11 = (static_cast<D3D11ComputeShader*>(mShaderStage[ST_Compute].get()))->ShaderD3D11;
		deviceContextD3D11->CSSetShader(shaderD3D11, nullptr, 0);

	}

	// Commit all shader resource
}

void D3D11ShaderProgram::Unbind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;

	if (mShaderStage[ST_Vertex])   deviceContextD3D11->VSSetShader(nullptr, nullptr, 0);
	if (mShaderStage[ST_Hull])     deviceContextD3D11->HSSetShader(nullptr, nullptr, 0);
	if (mShaderStage[ST_Domain])   deviceContextD3D11->DSSetShader(nullptr, nullptr, 0);
	if (mShaderStage[ST_Geomerty]) deviceContextD3D11->GSSetShader(nullptr, nullptr, 0);
	if (mShaderStage[ST_Pixel])    deviceContextD3D11->PSSetShader(nullptr, nullptr, 0);
	if (mShaderStage[ST_Compute])  deviceContextD3D11->CSSetShader(nullptr, nullptr, 0);
}


}