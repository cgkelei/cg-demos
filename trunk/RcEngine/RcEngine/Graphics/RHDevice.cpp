#include <Graphics/RHDevice.h>
#include <Graphics/RHFactory.h>

namespace RcEngine {

RHDevice::RHDevice( RenderDeviceType deviceType )
	: mDeviceType(deviceType),
	  mRenderFactory(nullptr),
	  mCurrentFrontStencilRef(0),
	  mCurrentBackStencilRef(0),
	  mCurrentBlendFactor(ColorRGBA::Black),
	  mCurrentSampleMask(0)
{

}

RHDevice::~RHDevice( void )
{
	SAFE_DELETE(mRenderFactory);
}





}