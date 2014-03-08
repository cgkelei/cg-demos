#include "EffectApp.h"
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Resource/ResourceManager.h>
#include <Core/Context.h>


EffectApp::EffectApp(const String& config)
	:Application(config)
{
}


EffectApp::~EffectApp(void)
{
}

void EffectApp::Initialize()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();

	auto material = resMan.GetResourceByName(RT_Material, "SkinModelUBO.material.xml", "General");
	material->Load();
}

void EffectApp::InitGUI()
{
	
}

void EffectApp::LoadContent()
{

}

void EffectApp::Update( float deltaTime )
{

}

void EffectApp::Render()
{

}

void EffectApp::CalculateFrameRate()
{

}


int main(int argc, char* argv[])
{
	EffectApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}