#ifndef OpenGLShader_h__
#define OpenGLShader_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/Shader.h>


namespace RcEngine {

class _OpenGLExport OpenGLShader : public Shader
{
public:
	OpenGLShader(ShaderType shaderType);
	virtual ~OpenGLShader();

	void Release();
	
	bool Compile(const String& source, const String& entryPoint = "");

	GLuint GetShaderObject() const { return mOGLShaderObject; }

private:
	GLuint mOGLShaderObject;

};

}



#endif // OpenGLShader_h__
