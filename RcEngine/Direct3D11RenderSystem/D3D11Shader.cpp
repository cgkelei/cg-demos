#include "D3D11Shader.h"
#include <Core/Loger.h>
#include <Core/Exception.h>
#include <Core/Utility.h>

namespace RcEngine {

ID3D11Device* g_pd3d11Device = nullptr;
ID3D11DeviceContext* g_pDeviceContext = nullptr;

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
HRESULT CompileHLSL(const String& filename, const std::vector<ShaderMacro>& macros, 
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

	std::vector<D3D10_SHADER_MACRO> d3dMacro(macros.size());
	for (size_t i = 0; i < macros.size(); ++i)
	{
		d3dMacro[i].Name = macros[i].Name.c_str();
		d3dMacro[i].Definition = macros[i].Definition.c_str();
	}

	// Convert to wchar
	std::wstring wfilename;
	Convert(wfilename, filename);

	hr = D3DCompileFromFile(wfilename.c_str(), &d3dMacro[0], D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), 
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

//////////////////////////////////////////////////////////////////////////
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
	
	HRESULT hr = g_pd3d11Device->CreateVertexShader(&ShaderCode[0], ShaderCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLoger::LogError("HLSL Vertex Shader %s compile failed\n", filename.c_str());
		return false;
	}
	
	return true;
}

bool D3D11VertexShader::LoadFromFile( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, entryPoint, "vs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_pd3d11Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
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

	HRESULT hr = g_pd3d11Device->CreateHullShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11HullShader::LoadFromFile( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, entryPoint, "hs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_pd3d11Device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
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

	HRESULT hr = g_pd3d11Device->CreateDomainShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11DomainShader::LoadFromFile( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, entryPoint, "ds_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_pd3d11Device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
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

	HRESULT hr = g_pd3d11Device->CreateGeometryShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11GeometryShader::LoadFromFile( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, entryPoint, "gs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_pd3d11Device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
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

	HRESULT hr = g_pd3d11Device->CreatePixelShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11PixelShader::LoadFromFile( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, entryPoint, "ps_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_pd3d11Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);
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

	HRESULT hr = g_pd3d11Device->CreateComputeShader(&byteCode[0], byteCode.size(), nullptr, &ShaderD3D11);
	if (FAILED(hr))
	{
		EngineLoger::LogError("HLSL Hull Shader %s compile failed\n", filename.c_str());
		return false;
	}

	return true;
}

bool D3D11ComputeShader::LoadFromFile( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, entryPoint, "cs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_pd3d11Device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		//D3D11_VERRY(hr);
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////
//D3D11ShaderProgram::D3D11ShaderProgram( Effect& effect )
//	: ShaderProgram(effect)
//{
//
//}
//
//D3D11ShaderProgram::~D3D11ShaderProgram()
//{
//
//}
//
//void D3D11ShaderProgram::Bind()
//{
//	if (mShaderStage[ST_Vertex])
//	{
//		ID3D11VertexShader* shaderD3D11 = (static_cast<D3D11VertexShader*>(mShaderStage[ST_Vertex].get()))->ShaderD3D11;
//		g_pDeviceContext->VSSetShader(shaderD3D11, nullptr, 0);
//	
//		// input layout
//	}
//
//	if (mShaderStage[ST_Hull])
//	{
//		ID3D11HullShader* shaderD3D11 = (static_cast<D3D11HullShader*>(mShaderStage[ST_Hull].get()))->ShaderD3D11;
//		g_pDeviceContext->HSSetShader(shaderD3D11, nullptr, 0);
//	}
//
//	if (mShaderStage[ST_Domain])
//	{
//		ID3D11DomainShader* shaderD3D11 = (static_cast<D3D11DomainShader*>(mShaderStage[ST_Domain].get()))->ShaderD3D11;
//		g_pDeviceContext->DSSetShader(shaderD3D11, nullptr, 0);
//	}
//
//	if (mShaderStage[ST_Geomerty])
//	{
//		ID3D11GeometryShader* shaderD3D11 = (static_cast<D3D11GeometryShader*>(mShaderStage[ST_Geomerty].get()))->ShaderD3D11;
//		g_pDeviceContext->GSSetShader(shaderD3D11, nullptr, 0);
//	}
//
//	if (mShaderStage[ST_Pixel])
//	{
//		ID3D11PixelShader* shaderD3D11 = (static_cast<D3D11PixelShader*>(mShaderStage[ST_Pixel].get()))->ShaderD3D11;
//		g_pDeviceContext->PSSetShader(shaderD3D11, nullptr, 0);
//	}
//
//	if (mShaderStage[ST_Compute])
//	{
//		ID3D11ComputeShader* shaderD3D11 = (static_cast<D3D11ComputeShader*>(mShaderStage[ST_Compute].get()))->ShaderD3D11;
//		g_pDeviceContext->CSSetShader(shaderD3D11, nullptr, 0);
//
//	}
//
//	// Commit all shader resource
//}
//
//void D3D11ShaderProgram::Unbind()
//{
//	if (mShaderStage[ST_Vertex]) g_pDeviceContext->VSSetShader(nullptr, nullptr, 0);
//	if (mShaderStage[ST_Hull]) g_pDeviceContext->HSSetShader(nullptr, nullptr, 0);
//	if (mShaderStage[ST_Domain]) g_pDeviceContext->DSSetShader(nullptr, nullptr, 0);
//	if (mShaderStage[ST_Geomerty]) g_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
//	if (mShaderStage[ST_Pixel]) g_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
//	if (mShaderStage[ST_Compute]) g_pDeviceContext->CSSetShader(nullptr, nullptr, 0);
//}
//
//bool D3D11ShaderProgram::Link()
//{
//
//}

}