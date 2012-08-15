#include <Core/Context.h>
#include <MainApp/Application.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderDevice.h>

namespace RcEngine{

SINGLETON_DECL(Core::Context)

namespace Core{

Context::Context()
	: mInputSystem(nullptr), mApplication(nullptr), mRenderDevice(nullptr), mRenderFactory(nullptr)
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

void Context::SetRenderFactory( RenderFactory* factory )
{
	mRenderFactory = factory;
}

void Context::SetInputSystem( InputSystem* input )
{
	mInputSystem = input;
}

InputSystem* Context::GetInputSystemPtr()
{
	return mInputSystem;
}

InputSystem& Context::GetInputSystem()
{
	return *mInputSystem;
}

} // Namespace Core
} // Namespace RcEngine