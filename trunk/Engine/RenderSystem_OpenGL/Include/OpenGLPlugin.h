#pragma once
#ifndef _OpenGLPlugin__H
#define _OpenGLPlugin__H

#include "OpenGLPrerequisites.h"
#include "PluginManager.h"

namespace Engine
{
	namespace RenderSystem_OpenGL
	{
		class _OpenGLExport OpenGLPlugin : public Plugin
		{
		public:
			OpenGLPlugin(void);
			~OpenGLPlugin(void);

			virtual const String& GetName() const;

			virtual void Install();

			virtual void Initialise();

			virtual void Shutdown();

			virtual void Uninstall();

		private:
			OpenGLRenderSystem* mRenderSystem;

		};
	}
}

#endif // OpenGLPlugin_h__