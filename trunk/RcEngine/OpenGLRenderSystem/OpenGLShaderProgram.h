#ifndef OpenGLShaderProgram_h__
#define OpenGLShaderProgram_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/ShaderProgram.h>

namespace RcEngine {

class _OpenGLExport OpenGLShaderProgram : public ShaderProgram
{
public:
	OpenGLShaderProgram(Effect& effect);
	~OpenGLShaderProgram();

	void Release();
	void Bind();
	void Unbind();
	bool LinkProgram();

	GLuint GetUniformLocation(const char* name);
	GLuint GetAttributeLocation(const char* name);

	GLuint GetShaderProgramObject() const { return mOGLProgramObject; }

private:
	void CaptureAllParameter();

private:
	GLuint mOGLProgramObject;
};

}


#endif // OpenGLShaderProgram_h__
