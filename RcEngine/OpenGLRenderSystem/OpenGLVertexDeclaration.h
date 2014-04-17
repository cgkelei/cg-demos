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

	/**
	 * Return false for newly created VAO.
	 */
	bool GetOrGenVertexArrayOGL(GLuint& vao);

private:
	GLuint mVertexArrayOGL;
};

}

#endif // OpenGLVertexDeclaration_h__
