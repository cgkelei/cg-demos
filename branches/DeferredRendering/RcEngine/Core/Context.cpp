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
	//Safe_Delete(mInputSystem);
}

void Context::Init( std::string configFile )
{

}


void Context::SetRenderDevice( RenderDevice* device )
{
	assert(device);
	mRenderDevice = device;
}

void Context::SetApplication( Application* app )
{
	assert(app);
	mApplication = app;
}

void Context::SetRenderFactory( RenderFactory* factory )
{
	assert(factory);
	mRenderFactory = factory;
}

void Context::SetSceneManager( SceneManager* scene )
{
	mSceneManager = scene;
}



} // Namespace RcEngine