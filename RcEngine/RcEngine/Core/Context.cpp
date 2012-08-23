#include <Core/Context.h>
#include <MainApp/Application.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderDevice.h>
#include <Scene/SceneManager.h>

namespace RcEngine{

SINGLETON_DECL(Context)



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

void Context::SetSceneManager( SceneManager* scene )
{
	mSceneManager = scene;
}



} // Namespace RcEngine