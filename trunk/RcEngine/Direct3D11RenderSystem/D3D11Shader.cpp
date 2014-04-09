#include "D3D11Shader.h"
#include <Core/Loger.h>
#include <Core/Utility.h>

namespace RcEngine {

ID3D11Device* g_pd3d11Device = nullptr;

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
		d3dMacro[i].Name = macros[i].first.c_str();
		d3dMacro[i].Definition = macros[i].second.c_str();
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
	: Shader(ST_Vertex),
	  ShaderD3D11(nullptr)
{

}

D3D11VertexShader::~D3D11VertexShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11VertexShader::Compile( const std::vector<uint8_t>& bytecode )
{
	HRESULT hr = g_pd3d11Device->CreateVertexShader(&bytecode[0], bytecode.size(), nullptr, &ShaderD3D11);
	assert(SUCCEEDED(hr));

	// Keep shader code, need to create input layout
	ShaderCode = bytecode;
	return true;
}

bool D3D11VertexShader::Complie( const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;

	HRESULT hr = CompileHLSL(filename, macros, entryPoint, "vs_5_0", &shaderBlob);
	if (SUCCEEDED(hr))
	{
		hr = g_pd3d11Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ShaderD3D11);
		D3D11_VERRY(hr);

		ShaderCode.resize(shaderBlob->GetBufferSize());
		std::memcpy(&ShaderCode[0], shaderBlob->GetBufferPointer(), ShaderCode.size());
	}

	if (shaderBlob)
		shaderBlob->Release();

	return SUCCEEDED(hr);
}


D3D11HullShader::D3D11HullShader()
	: Shader(ST_Vertex),
	  ShaderD3D11(nullptr)
{

}

D3D11HullShader::~D3D11HullShader()
{
	SAFE_RELEASE(ShaderD3D11);
}



D3D11DomainShader::D3D11DomainShader()
	: Shader(ST_Vertex),
	  ShaderD3D11(nullptr)
{

}

D3D11DomainShader::~D3D11DomainShader()
{
	SAFE_RELEASE(ShaderD3D11);
}


D3D11GeometryShader::D3D11GeometryShader()
	: Shader(ST_Geomerty),
	  ShaderD3D11(nullptr)
{

}

D3D11GeometryShader::~D3D11GeometryShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

D3D11PixelShader::D3D11PixelShader()
	: Shader(ST_Pixel),
	  ShaderD3D11(nullptr)
{

}

D3D11PixelShader::~D3D11PixelShader()
{

}

D3D11ComputeShader::D3D11ComputeShader()
	: Shader(ST_Compute),
	  ShaderD3D11(nullptr)
{

}

D3D11ComputeShader::~D3D11ComputeShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

}