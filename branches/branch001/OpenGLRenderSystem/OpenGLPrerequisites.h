#ifndef OpenGLPrerequisites_h__
#define OpenGLPrerequisites_h__

//////////////////////////////////////////////////////////////////////////
#include <gl/glew.h>
#include <gl/GL.h>
#include <cg/cg.h>
#include <cg/cgGL.h>


//////////////////////////////////////////////////////////////////////////
#include <nvImage/include/nvImage.h>

class nv::Image;
typedef nv::Image Image;


//////////////////////////////////////////////////////////////////////////
#include "Core/Prerequisites.h"

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


namespace RcEngine
{
	namespace Render
	{
		class OpenGLRenderDevice;
		class OpenGLTexture;
		class OpenGLTexture1D;
		class OpenGLTexture2D;
		class OpenGLTexture3D;
		class OpenGLTextureCube;
		class OpenGLRenderEffect;
		class OpenGLRenderParameter;
		class OpenGLRenderTechnique;
		class OpenGLRenderPass;
		class OpenGLGraphicsBuffer;
		class OpenGLFrameBuffer;
	}
}


#endif // OpenGLPrerequisites_h__
