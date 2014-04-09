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

	// Create Shader from bytecode
	virtual bool Compile(const std::vector<uint8_t>& bytecode);

	// Create shader from file
	virtual bool Complie(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

public:
	ID3D11VertexShader* ShaderD3D11;
	std::vector<uint8_t> ShaderCode;
};

class _D3D11Export D3D11HullShader : public Shader
{
public:
	D3D11HullShader();
	~D3D11HullShader();

	bool Compile(const String& source, const std::vector<String>& includes,
		const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

	bool Compile(const std::vector<uint8_t>& bytecode);

public:
	ID3D11HullShader* ShaderD3D11;
};

class _D3D11Export D3D11DomainShader : public Shader
{
public:
	D3D11DomainShader();
	~D3D11DomainShader();

	bool Compile(const String& source, const std::vector<String>& includes,
		const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

	bool Compile(const std::vector<uint8_t>& bytecode);

public:
	ID3D11DomainShader* ShaderD3D11;
};

class _D3D11Export D3D11GeometryShader : public Shader
{
public:
	D3D11GeometryShader();
	~D3D11GeometryShader();

	bool Compile(const String& source, const std::vector<String>& includes,
		const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

	bool Compile(const std::vector<uint8_t>& bytecode);

public:
	ID3D11GeometryShader* ShaderD3D11;
};

class _D3D11Export D3D11PixelShader : public Shader
{
public:
	D3D11PixelShader();
	~D3D11PixelShader();

	bool Compile(const String& source, const std::vector<String>& includes,
		const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

	bool Compile(const std::vector<uint8_t>& bytecode);

public:
	ID3D11PixelShader* ShaderD3D11;
};

class _D3D11Export D3D11ComputeShader : public Shader
{
public:
	D3D11ComputeShader();
	~D3D11ComputeShader();

	bool Compile(const String& source, const std::vector<String>& includes,
		const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

	bool Compile(const std::vector<uint8_t>& bytecode);

public:
	ID3D11ComputeShader* ShaderD3D11;
};

}

#endif // D3D11Shader_h__
