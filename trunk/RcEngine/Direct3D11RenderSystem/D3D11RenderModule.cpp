#include "D3D11RenderModule.h"
#include "D3D11RenderDevice.h"

namespace RcEngine {

D3D11RenderModule::D3D11RenderModule(void)
	: mRenderDevice(0)
{

}


D3D11RenderModule::~D3D11RenderModule(void)
{
}

const String& D3D11RenderModule::GetName() const
{
	static String DeviceName("Direct3D 11");
	return DeviceName;
}

RenderDevice* D3D11RenderModule::GetRenderDevice()
{
	return mRenderDevice;
}

void D3D11RenderModule::Initialise()
{
	mRenderDevice = new D3D11RenderDevice();
}

void D3D11RenderModule::Shutdown()
{
	delete mRenderDevice;
	mRenderDevice = 0;
}

}