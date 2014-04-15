#include "D3D11RenderModule.h"
#include "D3D11Device.h"

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

RHDevice* D3D11RenderModule::GetRenderDevice()
{
	return mRenderDevice;
}

void D3D11RenderModule::Initialise()
{
	RenderSettings settings;
	mRenderDevice = new D3D11Device(settings);
}

void D3D11RenderModule::Shutdown()
{
	delete mRenderDevice;
	mRenderDevice = 0;
}

}