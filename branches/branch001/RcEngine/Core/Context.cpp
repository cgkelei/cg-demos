#include "Core/Context.h"
#include "MainApp/Application.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/RenderDevice.h"

namespace RcEngine{

	SINGLETON_DECL(Core::Context)

		namespace Core{

			Context::Context()
			{

			}

			Context::~Context()
			{

			}

			void Context::Init( std::string configFile )
			{

			}

			RenderDevice& Context::GetRenderDevice()
			{
				return *mRenderDevice;
			}

			RenderFactory& Context::GetRenderFactory()
			{
				return *mRenderFactory;
			}

			RenderDevice* Context::GetRenderDevicePtr()
			{
				return mRenderDevice;
			}

			RenderFactory* Context::GetRenderFactoryPtr()
			{
				return mRenderFactory;
			}

			Application* Context::GetApplicationPtr()
			{
				return mApplication;
			}

			Application& Context::GetApplication()
			{
				return *mApplication;
			}

			void Context::SetRenderDevice( RenderDevice* device )
			{
				mRenderDevice = device;
			}

			void Context::SetApplication( Application* app )
			{
				mApplication = app;
			}

	} // Namespace Core
} // Namespace RcEngine