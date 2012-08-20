#ifndef EngineContext_h__
#define EngineContext_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine{

class _ApiExport Context : public Singleton<Context>
{
public:
	SINGLETON_DECL_HEADER(Context)

public:
	Context();
	~Context();

	void Init(std::string configFile);

	void SetRenderDevice(RenderDevice* device);
	RenderDevice& GetRenderDevice();
	RenderDevice* GetRenderDevicePtr();

	void SetRenderFactory(RenderFactory* factory);
	RenderFactory& GetRenderFactory();
	RenderFactory* GetRenderFactoryPtr(); 

	void SetApplication(Application* app);
	Application* GetApplicationPtr();
	Application& GetApplication();

	void SetInputSystem(InputSystem* input);
	InputSystem* GetInputSystemPtr();
	InputSystem& GetInputSystem();


private:
	RenderDevice* mRenderDevice;
	InputSystem* mInputSystem;
	RenderFactory* mRenderFactory;

	Application* mApplication;
};



} // Namespace RcEngine

#endif // EngineContext_h__
