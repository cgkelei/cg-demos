#ifndef D3D11RenderModule_h__
#define D3D11RenderModule_h__

#include "D3D11Prerequisites.h"
#include <Graphics/IRenderModule.h>

namespace RcEngine {

class D3D11RenderModule : public IRenderModule
{
public:
	D3D11RenderModule(void);
	~D3D11RenderModule(void);

	const String& GetName() const const ;
	RenderDevice* GetRenderDevice();

	void Initialise();
	void Shutdown();

private:
	RenderDevice* mRenderDevice;
};

}




#endif // D3D11RenderModule_h__
