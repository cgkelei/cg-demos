#ifndef D3D11Shader_h__
#define D3D11Shader_h__

#include "D3D11Prerequisites.h"
#include <Graphics/RHResource.h>

namespace RcEngine {

class _D3D11Export D3D11VertexShader : public RHShader
{
public:
	D3D11VertexShader();
	~D3D11VertexShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

public:
	ID3D11VertexShader* ShaderD3D11;
	std::vector<uint8_t> ShaderCode;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11HullShader : public RHShader
{
public:
	D3D11HullShader();
	~D3D11HullShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

public:
	ID3D11HullShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11DomainShader : public RHShader
{
public:
	D3D11DomainShader();
	~D3D11DomainShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

public:
	ID3D11DomainShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11GeometryShader : public RHShader
{
public:
	D3D11GeometryShader();
	~D3D11GeometryShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

public:
	ID3D11GeometryShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11PixelShader : public RHShader
{
public:
	D3D11PixelShader();
	~D3D11PixelShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

public:
	ID3D11PixelShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11ComputeShader : public RHShader
{
public:
	D3D11ComputeShader();
	~D3D11ComputeShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

public:
	ID3D11ComputeShader* ShaderD3D11;
};


//////////////////////////////////////////////////////////////////////////
//class _D3D11Export D3D11ShaderProgram : public RHShaderProgram
//{
//public:
//	D3D11ShaderProgram(Effect& effect);
//	~D3D11ShaderProgram();
//
//	virtual bool Link();
//	virtual void Bind();
//	virtual void Unbind();
//
//protected:
//	std::vector<std::function<void()>> mParameterBinds;
//};

}

#endif // D3D11Shader_h__
