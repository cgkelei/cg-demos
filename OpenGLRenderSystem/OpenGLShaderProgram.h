#ifndef OpenGLShaderProgram_h__
#define OpenGLShaderProgram_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/ShaderProgram.h>

namespace RcEngine {

class EffectConstantBuffer;

class _OpenGLExport OpenGLShaderProgram : public ShaderProgram
{
	//struct ParameterAssignment
	//{
	//	union ParameterType
	//	{
	//		EffectParameter* GlobalParam;
	//		EffectConstantBuffer* UniformBlock;
	//	};

	//	size_t LastModifyTimeStamp;
	//	std::function<void()> AssignmentFunc;
	//};

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
	void AddParameterBind(GLint location, EffectParameter* effectParam, GLsizei arrSize);
	void AddConstantBufferBind(GLuint bindingSlot, EffectConstantBuffer* effectCBuffer);

private:
	GLuint mOGLProgramObject;
	//vector<ParameterBind> mParameterBinds;
	
	std::vector<std::function<void()>> mParameterBinds;
};

}


#endif // OpenGLShaderProgram_h__
