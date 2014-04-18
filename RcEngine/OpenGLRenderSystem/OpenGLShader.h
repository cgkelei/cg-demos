#ifndef OpenGLShader_h__
#define OpenGLShader_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHResource.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

struct _OpenGLExport UniformParameter
{
	String Name;
	EffectParameterType Type;
	int32_t ArraySize;
	GLint Location; // -1 for uniform block parameter
	
	int32_t Offset; 
	int32_t ArrayStride;
	int32_t MatrixStride;
};

class _OpenGLExport OpenGLShader : public RHShader
{
public:
	OpenGLShader(ShaderType shaderType);
	virtual ~OpenGLShader();

	virtual bool LoadFromByteCode(const String& filename);
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint = "");

private:
	void ShaderReflect();

private:
	GLuint mShaderOGL;
	std::vector<UniformParameter> ShaderParameters;
};

//class _OpenGLExport OpenGLShaderPipeline : public ShaderProgram
//{
//public:
//	OpenGLShaderPipeline(Effect& effect);
//	~OpenGLShaderPipeline();
//
//	virtual void Bind();
//	virtual void Unbind();
//
//public:
//	GLuint PipelineOGL;
//};


}



#endif // OpenGLShader_h__
