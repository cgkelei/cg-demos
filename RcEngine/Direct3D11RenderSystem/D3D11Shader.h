#ifndef D3D11Shader_h__
#define D3D11Shader_h__

#include "D3D11Prerequisites.h"
#include <Graphics/Shader.h>

namespace RcEngine {

class _D3D11Export D3D11VertexShader : public Shader
{
public:
	D3D11VertexShader();
	~D3D11VertexShader();

	void Release() {}
	bool Compile(const String& source, const String& entryPoint = "");

public:
	ID3D11VertexShader* ShaderD3D11;
	std::vector<uint8_t> mCode;
};

class _D3D11Export D3D11HullShader : public Shader
{
public:
	D3D11HullShader();
	~D3D11HullShader();

	void Release() {}
	bool Compile(const String& source, const String& entryPoint = "");

public:
	ID3D11HullShader* ShaderD3D11;
};

class _D3D11Export D3D11DomainShader : public Shader
{
public:
	D3D11DomainShader();
	~D3D11DomainShader();

	void Release() {}
	bool Compile(const String& source, const String& entryPoint = "");

public:
	ID3D11DomainShader* ShaderD3D11;
};

class _D3D11Export D3D11GeometryShader : public Shader
{
public:
	D3D11GeometryShader();
	~D3D11GeometryShader();

	void Release() {}
	bool Compile(const String& source, const String& entryPoint = "");

public:
	ID3D11GeometryShader* ShaderD3D11;
};

class _D3D11Export D3D11PixelShader : public Shader
{
public:
	D3D11PixelShader();
	~D3D11PixelShader();

	void Release() {}
	bool Compile(const String& source, const String& entryPoint = "");

public:
	ID3D11PixelShader* ShaderD3D11;
};

class _D3D11Export D3D11ComputeShader : public Shader
{
public:
	D3D11ComputeShader();
	~D3D11ComputeShader();

	void Release() {}
	bool Compile(const String& source, const String& entryPoint = "");

public:
	ID3D11ComputeShader* ShaderD3D11;
};




}

#endif // D3D11Shader_h__
