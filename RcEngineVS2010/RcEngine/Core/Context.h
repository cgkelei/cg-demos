#ifndef EngineContext_h__
#define EngineContext_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine{

class Application;

class _ApiExport Context : public Singleton<Context>
{
public:
	SINGLETON_DECL_HEADER(Context)

public:
	Context();
	~Context();

	void Init(std::string configFile);

	void SetRenderDevice(RenderDevice* device);
	RenderDevice& GetRenderDevice()							{ assert(mRenderDevice); return *mRenderDevice; }
	RenderDevice* GetRenderDevicePtr()						{ assert(mRenderDevice); return mRenderDevice; }

	void SetRenderFactory(RenderFactory* factory);
	RenderFactory& GetRenderFactory()						{ assert(mRenderFactory); return *mRenderFactory; }
	RenderFactory* GetRenderFactoryPtr()					{ assert(mRenderFactory); return mRenderFactory; }

	void SetApplication(Application* app);
	Application* GetApplicationPtr()						{ assert(mApplication); return mApplication; }
	Application& GetApplication()							{ assert(mApplication); return *mApplication; }

	void SetInputSystem(InputSystem* input);
	InputSystem* GetInputSystemPtr()						{ assert(mInputSystem); return mInputSystem; }
	InputSystem& GetInputSystem()							{ assert(mInputSystem); return *mInputSystem; }

	void SetSceneManager( SceneManager* scene );
	SceneManager* GetSceneManagerPtr()						{ assert(mSceneManager); return mSceneManager; }
	SceneManager& GetSceneManager()							{ assert(mSceneManager); return *mSceneManager; }


private:
	RenderDevice* mRenderDevice;
	InputSystem* mInputSystem;
	RenderFactory* mRenderFactory;
	SceneManager* mSceneManager;
	Application* mApplication;
};



} // Namespace RcEngine

#endif // EngineContext_h__
