#ifndef OpenGLShader_h__
#define OpenGLShader_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHResource.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

struct GlobalParam
{
	String Name;
	GLint Location;
	GLuint ArraySize;
	EffectParameterType Type;
};

struct UniformBufferParam
{
	String Name;
	uint32_t BufferSize;
};

struct SRVParam
{
	String Name;
	GLuint Binding;
};

struct UAVParam
{
	String Name;
	GLuint Binding;
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
	std::vector<GlobalParam> mGlobalParams;
	std::vector<UniformBufferParam> mUniformBuffers;
};

class _OpenGLExport OpenGLShaderPipeline : public RHShaderPipeline
{
public:
	OpenGLShaderPipeline();
	~OpenGLShaderPipeline();

	virtual void OnBind();
	virtual void OnUnbind();

protected:

};


}



#endif // OpenGLShader_h__
