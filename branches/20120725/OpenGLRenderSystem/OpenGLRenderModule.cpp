#include "OpenGLRenderModule.h"
#include "OpenGLRenderDevice.h"

namespace RcEngine
{
	namespace Render
	{
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
			mRenderDevice = new OpenGLRenderDevice();
		}

		void OpenGLRenderModule::Shutdown()
		{
			delete mRenderDevice;
			mRenderDevice = 0;
		}


		RenderDevice* OpenGLRenderModule::GetRenderDevice()
		{
			return mRenderDevice;
		}

	}
}

