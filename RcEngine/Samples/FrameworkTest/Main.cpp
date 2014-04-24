#include <MainApp/Application.h>
#include <Graphics/RHFrameBuffer.h>
#include <Graphics/RHDevice.h>
#include <Graphics/RHFactory.h>
#include <Core/Environment.h>

using namespace RcEngine;

class FrameApp : public Application
{
public:
	FrameApp(const String& config)
		: Application(config)
	{

	}

	virtual ~FrameApp(void)
	{

	}

protected:
	void Initialize()
	{
		RHFactory* factory = Environment::GetSingleton().GetRHFactory();

		auto ps = factory->LoadShaderFromFile(ST_Pixel, "E:/RenderCascadeScene.hlsl", NULL, 0, "PSMain");


	}


	void LoadContent()
	{

	}

	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{

	}

	void Render()
	{
		RHDevice* device = Environment::GetSingleton().GetRHDevice();

		shared_ptr<RHFrameBuffer> screenFrameBuffer = device->GetScreenFrameBuffer();


		device->BindFrameBuffer(screenFrameBuffer);
		screenFrameBuffer->Clear(CF_Color | CF_Depth, ColorRGBA(1, 0, 0, 1), 1.0f, 0);


		screenFrameBuffer->SwapBuffers();
	}

	void CalculateFrameRate()
	{

	}

	void WindowResize(uint32_t width, uint32_t height)
	{

	}

protected:
	int mFramePerSecond;

};


int main()
{
	FrameApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}