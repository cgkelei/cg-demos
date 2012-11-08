#ifndef OpenGLShaderProgram_h__
#define OpenGLShaderProgram_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/ShaderProgram.h>

namespace RcEngine {

class _OpenGLExport OpenGLShaderProgram : public ShaderProgram
{
	struct ParameterBind
	{
		String Name;
		EffectParameterType Type;
		bool IsArray;
		GLint Location;
		std::function<void()> ShaderParamSetFunc;
		EffectParameter* EffectParameter;
	};

public:
	OpenGLShaderProgram(Effect& effect);
	~OpenGLShaderProgram();

	void Release();
	void Bind();
	void Unbind();
	bool LinkProgram();
   shared_ptr<ShaderProgram> Clone(Effect& effect);

	GLuint GetUniformLocation(const char* name);
	GLuint GetAttributeLocation(const char* name);

	GLuint GetShaderProgramObject() const { return mOGLProgramObject; }

private:
	void CaptureAllParameter();
	ParameterBind GetShaderParamBindFunc(GLint location, EffectParameter* effectParam, bool isArray);

private:
	GLuint mOGLProgramObject;
	vector<ParameterBind> mParameterBinds;
};

}


#endif // OpenGLShaderProgram_h__
