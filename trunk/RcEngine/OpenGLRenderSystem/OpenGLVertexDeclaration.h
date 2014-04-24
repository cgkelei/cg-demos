#ifndef OpenGLVertexDeclaration_h__
#define OpenGLVertexDeclaration_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHVertexDeclaration.h>

namespace RcEngine {

class _OpenGLExport OpenGLVertexDeclaration : public RHVertexDeclaration
{
public:
	OpenGLVertexDeclaration(const RHVertexElement* element, uint32_t count);
	~OpenGLVertexDeclaration();
	
	inline GLuint GetVertexArrayOGL() const { return mVertexArrayOGL; }
	
	void CreateVertexArrayOGL(const RHOperation& operation, const RHShader& vertexShader);

private:
	GLuint mVertexArrayOGL;
};

}

#endif // OpenGLVertexDeclaration_h__
