#ifndef D3D11Shader_h__
#define D3D11Shader_h__

#include "D3D11Prerequisites.h"
#include <Graphics/RHResource.h>

namespace RcEngine {

struct InputSignature
{
	String Semantic;
	uint32_t SemanticIndex;
};

struct SRVParam
{
	String Name;
	uint32_t Binding;
	EffectParameterType Type;
};

struct UAVParam
{
	String Name;
	uint32_t Binding;
	EffectParameterType Type;
};

struct UniformParam
{
	String Name;
	uint32_t Offset;
	uint32_t ArraySize;
	EffectParameterType Type;
};

struct UniformBufferParam
{
	String Name;
	uint32_t BufferSize;
	uint32_t Binding;
	vector<UniformParam> BufferVariables;
};

struct SamplerParam
{
	String Name;
	uint32_t Binding;
};


class _D3D11Export D3D11Shader : public RHShader
{
public:
	D3D11Shader(ShaderType type) : RHShader(type) {}
	virtual ~D3D11Shader() {}

public:
	vector<SRVParam> SRVParams;
	vector<UAVParam> UAVParams;
	vector<SamplerParam> SamplerParams;
	vector<UniformBufferParam> UniformBufferParams;

	friend class D3D11ShaderReflection;
	friend class D3D11ShaderPipeline;
};


//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11VertexShader : public D3D11Shader
{
public:
	D3D11VertexShader();
	~D3D11VertexShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11VertexShader* ShaderD3D11;
	vector<uint8_t> ShaderCode;
	vector<InputSignature> InputSignatures;
};

class _D3D11Export D3D11HullShader : public D3D11Shader
{
public:
	D3D11HullShader();
	~D3D11HullShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11HullShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11DomainShader : public D3D11Shader
{
public:
	D3D11DomainShader();
	~D3D11DomainShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11DomainShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11GeometryShader : public D3D11Shader
{
public:
	D3D11GeometryShader();
	~D3D11GeometryShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11GeometryShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11PixelShader : public D3D11Shader
{
public:
	D3D11PixelShader();
	~D3D11PixelShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11PixelShader* ShaderD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11ComputeShader : public D3D11Shader
{
public:
	D3D11ComputeShader();
	~D3D11ComputeShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount,const String& entryPoint = "");

public:
	ID3D11ComputeShader* ShaderD3D11;
};


////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11ShaderPipeline : public RHShaderPipeline
{
public:
	D3D11ShaderPipeline(Effect& effect);

	virtual void LinkPipeline();
	virtual void Bind();
	virtual void Unbind();

protected:
	std::vector<std::function<void()>> mParameterBinds; 
}; 

}

#endif // D3D11Shader_h__
