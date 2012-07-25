#ifndef IRenderModule_h__
#define IRenderModule_h__

#include "Core/IModule.h"

namespace RcEngine
{
	namespace Render
	{
		class _ApiExport IRenderModule : public Core::IModule
		{
		protected:
			IRenderModule();

		public:
			virtual ~IRenderModule();

			virtual RenderDevice* GetRenderDevice() = 0;
		};

	}
}

#endif // IRenderModule_h__
