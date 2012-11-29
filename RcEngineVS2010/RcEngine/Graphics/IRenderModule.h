#ifndef IRenderModule_h__
#define IRenderModule_h__

#include <Core/IModule.h>

namespace RcEngine {

class _ApiExport IRenderModule : public IModule
{
protected:
	IRenderModule();

public:
	virtual ~IRenderModule();

	virtual RenderDevice* GetRenderDevice() = 0;
};


} // Namespace RcEngine

#endif // IRenderModule_h__
