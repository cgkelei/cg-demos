#pragma once
#ifndef _D3D9RenderSystem__H
#define _D3D9RenderSystem__H


#include "D3D9Prerequisites.h"
#include "RenderSystem.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		class _D3D9Export D3D9RenderSystem : public RenderSystem
		{
		private:
			friend class D3D9Device;
			friend class D3D9DeviceManager;	

		public:
			D3D9RenderSystem(HINSTANCE hInst);
			virtual ~D3D9RenderSystem(void);

			const String& GetName() const; 


			void BeignFrame();
			void EndFrame();

			void SetViewport (Viewport *vp);

			void SetViewMatrix (const Matrix4f& m);
			void SetWorldMatrix (const Matrix4f& m);
			void SetProjectionMatrix (const Matrix4f& m);


			RenderWindow* Initialize(bool autoCreateWindow, const String& windowTitle = "Render Window"); 
			void Reinitialize () ;

			RenderWindow* CreateRenderWindow(const String &name, const RenderSettings& settings);

			void Shutdown();

			RenderStateManager* GetRenderStateManager(); 

			void SetAmbientLight(float r, float g, float b);
			void SetShadingType(ShadingType st);

			void SetVertexDeclaration(VertexDeclaration* decl);



			void Clear(uint32 buffers = CO_Target | CO_DepthBuffer, const ColorRGBA& colour = ColorRGBA::Black, 
				float depth = 1.0f, uint16 stencil = 0);

			static IDirect3D9* GetDirect3D9();
			static D3D9ResourceManager* GetResourceManager();
			static D3D9DeviceManager* GetDeviceManager();
			static IDirect3DDevice9* GetActiveD3D9Device();

		private:

			D3D9DriverList* GetDirect3DDrivers();

			HRESULT SetRenderState(D3DRENDERSTATETYPE state, DWORD value);


		private:
			
			// App Instance
			HINSTANCE mhInstance;

			// Direct3D Interface
			IDirect3D9*	mpD3D;		

			// Fast singleton access.
			static D3D9RenderSystem* msD3D9RenderSystem;

			// Fullscreen Anti-Aliasing
			uint32 mFSAASamples;
			String mFSAAHint;
			
			uint32 mLastVertexSourceCount;

			// List of D3D drivers installed (video cards)
			D3D9DriverList* mDriverList;

			// Currently active driver
			D3D9Driver* mActiveD3DDriver;

			D3D9DeviceManager* mDeviceManager;
			D3D9ResourceManager* mResourceManager;
			D3D9RenderStateManager* mRenderStateManager;

			
#pragma warning (push)
#pragma warning (disable : 4251)

			typedef std::vector<D3D9RenderWindow*> D3D9RenderWindowList;
			// List of additional windows after the first (swap chains)
			D3D9RenderWindowList mRenderWindows;

#pragma warning (pop)
		

		

		};
	}
}

#endif