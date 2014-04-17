#ifndef D3D11RenderModule_h__
#define D3D11RenderModule_h__

#include "D3D11Prerequisites.h"
#include <Core/IModule.h>

namespace RcEngine {

class RHDevice;

class _D3D11Export D3D11Module : public IModule
{
public:
	D3D11Module(void);
	~D3D11Module(void);

	const String& GetName() const;
	void Initialise();
	void Shutdown();

private:
	RHDevice* mRenderDevice;
};

}

#endif // D3D11RenderModule_h__
