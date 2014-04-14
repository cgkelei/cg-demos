#include "OpenGLVertexDeclaration.h"

namespace RcEngine {

OpenGLVertexDeclaration::OpenGLVertexDeclaration()
	: mVertexArrayOGL(0)
{
	
}

OpenGLVertexDeclaration::~OpenGLVertexDeclaration()
{
	if (mVertexArrayOGL)
		glDeleteVertexArrays(1, &mVertexArrayOGL);
}

bool OpenGLVertexDeclaration::GetOrGenVertexArrayOGL( GLuint& vao )
{
	bool bCreated = true;
	if (mVertexArrayOGL == 0)
	{
		glGenVertexArrays(1, &mVertexArrayOGL);
		bCreated = false;
	}
	
	vao = mVertexArrayOGL;
	return bCreated;
}

}