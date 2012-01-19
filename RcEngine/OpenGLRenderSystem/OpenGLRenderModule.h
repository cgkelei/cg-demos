#ifndef OpenGLRenderModule_h__
#define OpenGLRenderModule_h__

#include "Graphics/IRenderModule.h"
#include "OpenGLPrerequisites.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		class _OpenGLExport OpenGLRenderModule : public IRenderModule
		{
		public:
			OpenGLRenderModule(void);
			~OpenGLRenderModule(void);

			const std::string& GetName() const ;
			RenderDevice* GetRenderDevice();

			void Install();
			void Initialise();
			void Shutdown();
			void Uninstall();

		private:
			RenderDevice* mRenderDevice;
		};
	}
}


#endif // OpenGLRenderModule_h__