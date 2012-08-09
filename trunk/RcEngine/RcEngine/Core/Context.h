#ifndef EngineContext_h__
#define EngineContext_h__

#include "Prerequisites.h"
#include "Singleton.h"

namespace RcEngine{
	namespace Core{

		using Render::RenderDevice;
		using Render::RenderFactory;
		using Input::InputSystem;

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


	} // Namespace Core
} // Namespace RcEngine

#endif // EngineContext_h__
