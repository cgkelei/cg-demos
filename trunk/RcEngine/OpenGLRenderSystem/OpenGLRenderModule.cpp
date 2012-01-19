#include "OpenGLRenderModule.h"
#include "OpenGLRenderDevice.h"

namespace RcEngine
{
	namespace RenderSystem
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

		void OpenGLRenderModule::Install()
		{
			
			mRenderDevice = new OpenGLRenderDevice();
		}

		void OpenGLRenderModule::Initialise()
		{
			
		}

		void OpenGLRenderModule::Shutdown()
		{

		}

		void OpenGLRenderModule::Uninstall()
		{
			delete mRenderDevice;
			mRenderDevice = 0;
		}

		RenderDevice* OpenGLRenderModule::GetRenderDevice()
		{
			std::cout << "called";
			return mRenderDevice;
		}

	}
}

