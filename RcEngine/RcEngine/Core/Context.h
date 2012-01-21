#ifndef EngineContext_h__
#define EngineContext_h__

#include "Prerequisites.h"
#include "Singleton.h"

namespace RcEngine{
	namespace Core{

		using RenderSystem::RenderDevice;
		using RenderSystem::RenderFactory;

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
			

			RenderFactory& GetRenderFactory();
			RenderFactory* GetRenderFactoryPtr(); 

			void SetApplication(Application* app);
			Application* GetApplicationPtr();
			Application& GetApplication();


		private:
			RenderDevice* mRenderDevice;
			RenderFactory* mRenderFactory;
			Application* mApplication;
		};


	} // Namespace Core
} // Namespace RcEngine

#endif // EngineContext_h__
