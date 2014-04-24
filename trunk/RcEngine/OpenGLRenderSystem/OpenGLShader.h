#ifndef OpenGLShader_h__
#define OpenGLShader_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHResource.h>
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

class _OpenGLExport OpenGLShader : public RHShader
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
};

class _OpenGLExport OpenGLShaderPipeline : public RHShaderPipeline
{
public:
	OpenGLShaderPipeline(Effect& effect);

	virtual void OnBind();
	virtual void OnUnbind();
	virtual void LinkPipeline();

private:
	void AddUniformParamBind(GLuint shader, GLint location, EffectParameter* effectParam, GLsizei arrSize);
	void AddSRVParamBind(EffectParameter* effectParam, GLuint binding);
	void AddUAVParamBind(EffectParameter* effectParam, GLuint binding);

	void AddShaderResourceBind(GLuint shader, GLint location, GLuint binding, bool shaderStorage);

private:
	std::map<String, GLuint> mBindingCache;
	std::vector<std::function<void()>> mParameterBinds; 
};


}



#endif // OpenGLShader_h__
