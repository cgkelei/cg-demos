#include "OpenGLRenderModule.h"
#include "OpenGLDevice.h"

namespace RcEngine {

const static std::string ModuleName = "GL RenderSystem";

OpenGLRenderModule::OpenGLRenderModule(void)
	: mRenderDevice(0)
{
}


OpenGLRenderModule::~OpenGLRenderModule(void)
{
}

const std::string& OpenGLRenderModule::GetName() const
{
	return ModuleName;
}


void OpenGLRenderModule::Initialise()
{
	RenderSettings setting;
	mRenderDevice = new OpenGLDevice(setting);
}

void OpenGLRenderModule::Shutdown()
{
	delete mRenderDevice;
	mRenderDevice = 0;
}


RHDevice* OpenGLRenderModule::GetRenderDevice()
{
	return mRenderDevice;
}

}

