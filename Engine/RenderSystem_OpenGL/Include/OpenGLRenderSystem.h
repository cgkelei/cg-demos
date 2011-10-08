#pragma once
#ifndef _OpenGLRenderSystem__H
#define _OpenGLRenderSystem__H

#include "OpenGLPrerequisites.h"
#include "RenderSystem.h"

namespace Engine
{
	namespace RenderSystem_OpenGL
	{
		class _OpenGLExport OpenGLRenderSystem : public RenderSystem
		{
		public:
			OpenGLRenderSystem(void);
			~OpenGLRenderSystem(void);

			virtual void ShowMessage() const;


			void const String& GetName() const;
		};
	}
}


#endif 