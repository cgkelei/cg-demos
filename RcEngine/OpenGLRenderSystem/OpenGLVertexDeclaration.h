#ifndef OpenGLVertexDeclaration_h__
#define OpenGLVertexDeclaration_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHVertexDeclaration.h>

namespace RcEngine {

class _OpenGLExport OpenGLVertexDeclaration : public RHVertexDeclaration
{
public:
	OpenGLVertexDeclaration();
	~OpenGLVertexDeclaration();

	bool GetOrGenVertexArrayOGL(GLuint& vao);

private:
	GLuint mVertexArrayOGL;
};

}

#endif // OpenGLVertexDeclaration_h__
