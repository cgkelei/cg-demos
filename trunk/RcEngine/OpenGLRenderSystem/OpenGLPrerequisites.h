#ifndef OpenGLPrerequisites_h__
#define OpenGLPrerequisites_h__

//////////////////////////////////////////////////////////////////////////
#include <gl/glew.h>
#include <GL/wglew.h>
#include <gl/GL.h>


//////////////////////////////////////////////////////////////////////////
#include <nvImage/include/nvImage.h>

class nv::Image;
typedef nv::Image Image;


//////////////////////////////////////////////////////////////////////////
#include <Core/Prerequisites.h>

//////////////////////////////////////////////////////////////////////////
#ifndef BUILD_STATIC
#	ifdef OPENGLENGINEDLL_EXPORTS
#		define _OpenGLExport __declspec(dllexport)
#	else
#    	define _OpenGLExport __declspec(dllimport) 
#	endif
#else
#	define _OpenGLExport
#endif	

inline void CheckOpenGLError(const char* func, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %s, at %s:%i - for %s\n", gluErrorString(err), fname, line, func);
		abort();
	}
}

#ifdef _DEBUG
#define OGL_ERROR_CHECK() do { \
	CheckOpenGLError(__FUNCTION__, __FILE__, __LINE__); \
} while(0)
#else
	#define OGL_ERROR_CHECK() 
#endif 

namespace RcEngine
{

class OpenGLDevice;
class OpenGLTexture;
class OpenGLTexture1D;
class OpenGLTexture2D;
class OpenGLTexture3D;
class OpenGLTextureCube;
class OpenGLEffect;
class OpenGLEffectParameter;
class OpenGLEffectTechnique;
class OpenGLEffectPass;
class OpenGLGraphicsBuffer;
class OpenGLFrameBuffer;

}


#endif // OpenGLPrerequisites_h__
