#include "OpenGLVertexDeclaration.h"

namespace RcEngine {

OpenGLVertexDeclaration::OpenGLVertexDeclaration( const RHVertexElement* element, uint32_t count )
	: RHVertexDeclaration(element, count),
	  mVertexArrayOGL(0)
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