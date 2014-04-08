#include "D3D11Shader.h"

namespace RcEngine {

ID3D11Device* g_pd3d11Device = nullptr;

D3D11VertexShader::D3D11VertexShader()
	: Shader(ST_Vertex),
	  ShaderD3D11(nullptr)
{

}

D3D11VertexShader::~D3D11VertexShader()
{
	SAFE_RELEASE(ShaderD3D11);
}

bool D3D11VertexShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
{
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	std::vector<D3D10_SHADER_MACRO>
	D3DCompile(source.c_str(), source.length(), nullptr, )
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


bool D3D11HullShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
{

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


bool D3D11DomainShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
{

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

bool D3D11GeometryShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
{

}


D3D11PixelShader::D3D11PixelShader()
	: Shader(ST_Pixel),
	  ShaderD3D11(nullptr)
{

}

D3D11PixelShader::~D3D11PixelShader()
{

}

bool D3D11PixelShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
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

bool D3D11ComputeShader::Compile( const String& source, const String& entryPoint /*= ""*/ )
{

}

}