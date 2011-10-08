#pragma  once
#ifndef _OpenGLPrerequisites__H
#define _OpenGLPrerequisites__H

#include "Prerequisites.h"

namespace Engine
{
	namespace RenderSystem_OpenGL
	{

#ifndef BUILD_STATIC
#	ifdef OPENGLENGINEDLL_EXPORTS
#		define _OpenGLExport __declspec(dllexport)
#	else
#    	define _OpenGLExport __declspec(dllimport) 
#	endif
#else
#	define _OpenGLExport
#endif	


		class OpenGLRenderSystem;
	}
}

#endif