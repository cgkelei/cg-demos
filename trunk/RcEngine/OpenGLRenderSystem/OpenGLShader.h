#ifndef OpenGLShader_h__
#define OpenGLShader_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/Shader.h>

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

//struct _OpenGLExport UniformBuffer
//{
//	
//};


class _OpenGLExport OpenGLShader : public Shader
{
public:
	OpenGLShader(ShaderType shaderType);
	virtual ~OpenGLShader();
	
	virtual bool LoadFromByteCode(const String& filename) = 0;
	virtual bool LoadFromFile(const String& filename, const std::vector<Macro>& macros, const String& entryPoint = "") = 0;

private:
	void ShaderReflect();

public:
	GLuint ShaderOGL;
	std::vector<UniformParameter> ShaderParameters;
};

class _OpenGLExport OpenGLShaderPipeline : public ShaderProgram
{
public:
	OpenGLShaderPipeline(Effect& effect);
	~OpenGLShaderPipeline();

	virtual void Bind();
	virtual void Unbind();

private:
	struct GlobalCommit
	{
		GLuint ShaderOGL;
		GLuint ParamLocation;
	};

	struct UniformCommit
	{

	};
};


}



#endif // OpenGLShader_h__
