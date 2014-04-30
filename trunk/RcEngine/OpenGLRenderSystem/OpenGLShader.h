#ifndef OpenGLShader_h__
#define OpenGLShader_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsResource.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

struct InputSignature
{
	GLenum Type;
	GLint ArraySize; 
	GLuint AttributeSlot;
};

struct UniformParam
{
	String Name;
	GLint Location;
	GLuint ArraySize;
	EffectParameterType Type;
};

struct UniformBufferParam
{
	String Name;
	GLint Location;
	uint32_t BufferSize;
	vector<UniformParam> BufferVariables;
};

struct SRVParam
{
	String Name;
	GLint Location;
	EffectParameterType Type;
};

struct UAVParam
{
	String Name;
	GLint Location;
	EffectParameterType Type;
};

// Forward declaration
class OpenGLShaderReflection;
class OpenGLShaderPipeline;

class _OpenGLExport OpenGLShader : public Shader
{
public:
	OpenGLShader(ShaderType shaderType);
	virtual ~OpenGLShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint = "");

private:
	friend class OpenGLShaderReflection;
	friend class OpenGLShaderPipeline;

	GLuint mShaderOGL;
	
	std::vector<SRVParam> mSRVParams;
	std::vector<UAVParam> mUAVParams;
	std::vector<UniformParam> mGlobalParams;
	std::vector<UniformBufferParam> mUniformBuffers;
	std::vector<InputSignature> mInputSignatures;
	std::map<String, String> mSamplerStates;
};

class _OpenGLExport OpenGLShaderPipeline : public ShaderPipeline
{
public:
	OpenGLShaderPipeline(Effect& effect);
	virtual ~OpenGLShaderPipeline();

	virtual void OnBind();
	virtual void OnUnbind();
	virtual bool LinkPipeline();

private:
	void AddUniformParamBind(GLuint shader, GLint location, EffectParameter* effectParam, GLsizei arrSize);
	void AddSRVParamBind(EffectParameter* effectParam, GLuint binding);
	void AddUAVParamBind(EffectParameter* effectParam, GLuint binding);

	void AddShaderResourceBind(GLuint shader, GLint location, GLuint binding, bool shaderStorage);

private:
	GLuint mPipelineOGL;
	std::vector<std::function<void()>> mParameterBinds; 
};


}



#endif // OpenGLShader_h__
