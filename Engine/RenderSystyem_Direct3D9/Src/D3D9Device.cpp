#include "D3D9Device.h"
#include "D3D9RenderWindow.h"
#include "D3D9RenderSystem.h"
#include "D3D9DeviceManager.h"
#include "Exception.h"
#include "EngineRoot.h"
#include "Utility.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		
		D3D9Device::D3D9Device(D3D9DeviceManager* deviceManager,
			UINT adapterNumber, 
			HMONITOR hMonitor, 
			D3DDEVTYPE devType, 
			DWORD behaviorFlags)
		{
			mpDeviceManager				= deviceManager;
			mpDevice					= NULL;		
			mAdapterNumber				= adapterNumber;
			mMonitor					= hMonitor;
			mDeviceType					= devType;
			mFocusWindow				= NULL;
			mBehaviorFlags				= behaviorFlags;	
			mD3D9DeviceCapsValid		= false;
			mDeviceLost					= false;
			ZeroMemory(&mPresentationParam, sizeof(D3DPRESENT_PARAMETERS));
			memset(&mD3D9DeviceCaps, 0, sizeof(mD3D9DeviceCaps));
			memset(&mCreationParams, 0, sizeof(mCreationParams));	

			mBackBuffer = NULL;
			mSwapChain = NULL;
			mDepthBuffer = NULL;
			mActiveRenderWindow = NULL;
			mAcquired = false;
		}

		//---------------------------------------------------------------------
		D3D9Device::~D3D9Device()
		{

		}


		void D3D9Device::AttachRenderWindow( D3D9RenderWindow* renderWindow )
		{
			mActiveRenderWindow = renderWindow;
		}

		void D3D9Device::DetachRenderWindow( D3D9RenderWindow* renderWindow )
		{
			mFocusWindow = NULL;
			ReleaseRenderWindowResource();
		}

		bool D3D9Device::IsDeviceLost()
		{
			return mDeviceLost;
		}

		IDirect3DDevice9* D3D9Device::GetD3D9Device()
		{
			return mpDevice;
		}

		D3DFORMAT D3D9Device::GetBackBufferFormat() const
		{
			return mPresentationParam.BackBufferFormat;
		}

		void D3D9Device::CreateD3D9Device()
		{
			D3D9RenderWindow* primaryRenderWindow = mActiveRenderWindow;

			mFocusWindow = primaryRenderWindow->GetWindowHandle();

			IDirect3D9* pD3D9 = D3D9RenderSystem::GetDirect3D9();
			HRESULT hr = NULL;


			// Try to create the device with hardware vertex processing. 
			mBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			hr = pD3D9->CreateDevice(mAdapterNumber, mDeviceType, mFocusWindow,
				mBehaviorFlags, &mPresentationParam, &mpDevice);

			if (FAILED(hr))
			{
				// Try a second time, may fail the first time due to back buffer count,
				// which will be corrected down to 1 by the runtime
				hr = pD3D9->CreateDevice(mAdapterNumber, mDeviceType, mFocusWindow,
					mBehaviorFlags, &mPresentationParam, &mpDevice);
			}

			// Case hardware vertex processing failed.
			if( FAILED( hr ) )
			{
				// Try to create the device with mixed vertex processing.
				mBehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
				mBehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;

				hr = pD3D9->CreateDevice(mAdapterNumber, mDeviceType, mFocusWindow,
					mBehaviorFlags, &mPresentationParam, &mpDevice);
			}
			
			if( FAILED( hr ) )
			{
				// try to create the device with software vertex processing.
				mBehaviorFlags &= ~D3DCREATE_MIXED_VERTEXPROCESSING;
				mBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				hr = pD3D9->CreateDevice(mAdapterNumber, mDeviceType, mFocusWindow,
					mBehaviorFlags, &mPresentationParam, &mpDevice);
			}

			if ( FAILED( hr ) )
			{
				// try reference device
				mDeviceType = D3DDEVTYPE_REF;
				hr = pD3D9->CreateDevice(mAdapterNumber, mDeviceType, mFocusWindow,
					mBehaviorFlags, &mPresentationParam, &mpDevice);

				if ( FAILED( hr ) )
				{
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
						"Cannot create device!", 
						"D3D9Device::CreateD3D9Device" );
				}
			}

			// Get current creation parameters caps.
			hr = mpDevice->GetCreationParameters(&mCreationParams);
			if ( FAILED(hr) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Error Get Creation Parameters", 
					"D3D9Device:CreateD3D9Device" );
			}

			mD3D9DeviceCapsValid = true;

			// Initialize device states.
			SetupDeviceStates();
		}

		

		void D3D9Device::SetupDeviceStates()
		{
			HRESULT hr = mpDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

			if (FAILED(hr)) 
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Unable to apply render state: D3DRS_SPECULARENABLE <- TRUE",
					"D3D9Device::setupDeviceStates");
			}		
		}

		void D3D9Device::ReleaseD3D9Device()
		{
			if(mpDevice != NULL)
			{
				// Release device.
				SAFE_RELEASE(mpDevice);	
			}
		}

		IDirect3DSurface9* D3D9Device::GetDepthBuffer()
		{
			return mDepthBuffer;
		}

		IDirect3DSurface9* D3D9Device::GetBackBuffer()
		{
			return mBackBuffer;
		}

		void D3D9Device::UpdateRenderWindowPresentation()
		{
			if(mActiveRenderWindow)
			{
				// Ask the render window to build it's own parameters.
				mActiveRenderWindow->BuildPresentParameters(&mPresentationParam);		
				if(IsAutoDepthStencil() == false)
				{
					mPresentationParam.EnableAutoDepthStencil = false;
				}
			}
		}

		void D3D9Device::ReleaseRenderWindowResource()
		{
			SAFE_RELEASE(mBackBuffer);
			SAFE_RELEASE(mDepthBuffer);
			SAFE_RELEASE(mSwapChain);
			mAcquired = false;
		}

		void D3D9Device::ValidateBackBufferSize( D3D9RenderWindow* renderWindow )
		{
			// Case size has been changed.
			if (renderWindow->GetWidth() !=  mPresentationParam.BackBufferWidth ||
				renderWindow->GetHeight() !=  mPresentationParam.BackBufferHeight)
			{			
				if (renderWindow->GetWidth() > 0)
					mPresentationParam.BackBufferWidth = renderWindow->GetWidth();

				if (renderWindow->GetHeight() > 0)
					mPresentationParam.BackBufferHeight = renderWindow->GetHeight();

				Invalidate(renderWindow);
			}				
		}

		bool D3D9Device::ValidateDisplayMonitor( D3D9RenderWindow* renderWindow )
		{
			return true;
		}

		bool D3D9Device::ValidateDeviceState( D3D9RenderWindow* renderWindow )
		{
			HRESULT hr;

			hr = mpDevice->TestCooperativeLevel();	

			// Case device is not valid for rendering. 
			if (FAILED(hr))
			{					
				// device lost, and we can't reset
				// can't do anything about it here, wait until we get 
				// D3DERR_DEVICENOTRESET; rendering calls will silently fail until 
				// then (except Present, but we ignore device lost there too)
				if (hr == D3DERR_DEVICELOST)
				{						
					ReleaseRenderWindowResource();
					NotifyDeviceLost();							
					return false;
				}

				// device lost, and we can reset
				else if (hr == D3DERR_DEVICENOTRESET)
				{					
					bool deviceRestored = Reset();

					// Device was not restored yet.
					if (deviceRestored == false)
					{
						// Wait a while
						Sleep(50);					
						return false;
					}																								
				}						
			}

			// Render window resources explicitly invalidated. (Resize or window mode switch) 
			if (mAcquired == false)
			{
				if (mActiveRenderWindow == renderWindow)
				{
					bool deviceRestored = Reset();

					// Device was not restored yet.
					if (deviceRestored == false)
					{
						// Wait a while
						Sleep(50);					
						return false;
					}	
				}
				else
				{
					Acquire(renderWindow);
				}
			}

			return true;
		}

		bool D3D9Device::Validate( D3D9RenderWindow* renderWindow )
		{
			// Validate that the render window should run on this device.
			if (false == ValidateDisplayMonitor(renderWindow))
				return false;

			// Validate that the render window dimensions matches to back buffer dimensions.
			ValidateBackBufferSize(renderWindow);

			// Validate that this device is in valid rendering state.
			if (false == ValidateDeviceState(renderWindow))
				return false;

			return true;
		}

		void D3D9Device::Invalidate( D3D9RenderWindow* renderWindow )
		{
			mAcquired = false;
		}

		bool D3D9Device::Reset()
		{
			HRESULT hr;

			// Check that device is in valid state for reset.
			hr = mpDevice->TestCooperativeLevel();
			if (hr == D3DERR_DEVICELOST ||
				hr == D3DERR_DRIVERINTERNALERROR)
			{
				return false;
			}

		

			//D3D9RenderSystem* renderSystem = static_cast<D3D9RenderSystem*>(Root::getSingleton().getRenderSystem());

			//// Inform all resources that device lost.
			//D3D9RenderSystem::getResourceManager()->notifyOnDeviceLost(mpDevice);

			//// Notify all listener before device is rested
			//renderSystem->notifyOnDeviceLost(this);

			//// Release all automatic temporary buffers and free unused
			//// temporary buffers, so we doesn't need to recreate them,
			//// and they will reallocate on demand. This save a lot of
			//// release/recreate of non-managed vertex buffers which
			//// wasn't need at all.
			//HardwareBufferManager::getSingleton()._releaseBufferCopies(true);


			//// Cleanup depth stencils surfaces.
			//renderSystem->_cleanupDepthStencils(mpDevice);

			UpdateRenderWindowPresentation();	
			ReleaseRenderWindowResource();


			ClearDeviceStreams();


			// Reset the device using the presentation parameters.
			hr = mpDevice->Reset(&mPresentationParam);

			if (hr == D3DERR_DEVICELOST)
			{
				// Don't continue
				return false;
			}
			else if (FAILED(hr))
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Cannot reset device!", 
					"D3D9RenderWindow::reset");
			}

			mDeviceLost = false;

			// Initialize device states.
			SetupDeviceStates();

		
			//AcquireRenderWindowResources(it);

			//D3D9Device* pCurActiveDevice = mpDeviceManager->getActiveDevice();

			//mpDeviceManager->setActiveDevice(this);

			//// Inform all resources that device has been reset.
			//D3D9RenderSystem::getResourceManager()->notifyOnDeviceReset(mpDevice);

			//mpDeviceManager->setActiveDevice(pCurActiveDevice);

			//renderSystem->notifyOnDeviceReset(this);

			//// UnLock access to rendering device.
			//D3D9RenderSystem::getResourceManager()->unlockDeviceAccess();

			return true;
		}

		void D3D9Device::AcquireRenderWindowResources()
		{
			ReleaseRenderWindowResource();
	
			// The swap chain is already created by the device
			HRESULT hr = mpDevice->GetSwapChain(0, 
				&mSwapChain);
			if (FAILED(hr)) 
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Unable to get the swap chain",
					"D3D9RenderWindow::acquireRenderWindowResources");
			}
	
			// Store references to buffers for convenience, Specifies a nonstereo swap chain. 	
			hr = mSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &mBackBuffer);
			
			if (FAILED(hr)) 
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Unable to retrieves a back buffer from the device's swap chain",
					"D3D9RenderWindow::AcquireRenderWindowResources");
			}


			if (mActiveRenderWindow->IsDepthBuffered())
			{
				// depth buffer is created for the main swap chain
				if (IsAutoDepthStencil())
				{
					hr = mpDevice->GetDepthStencilSurface(&mDepthBuffer);
					if (FAILED(hr)) 
					{
						ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
							"Unable to retrieves a depth buffer from the device's swap chain",
							"D3D9RenderWindow::AcquireRenderWindowResources");
					}
				}
				else
				{
					uint32 targetWidth  = mActiveRenderWindow->GetWidth();
					uint32 targetHeight = mActiveRenderWindow->GetHeight();

					if (targetWidth == 0)
						targetWidth = 1;

					if (targetHeight == 0)
						targetHeight = 1;

					HRESULT hr = mpDevice->CreateDepthStencilSurface(
						targetWidth, targetHeight,
						mPresentationParam.AutoDepthStencilFormat,
						mPresentationParam.MultiSampleType,
						mPresentationParam.MultiSampleQuality, 
						(mPresentationParam.Flags & D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL),
						&mDepthBuffer, NULL
						);

					if (FAILED(hr)) 
					{
						ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
							"Unable to create a depth buffer for the swap chain",
							"D3D9RenderWindow::AcquireRenderWindowResources");
					}

					mpDevice->SetDepthStencilSurface(mDepthBuffer);
					
				}
			}

			mAcquired= true; 
		}

		

		bool D3D9Device::Acquire()
		{
			UpdateRenderWindowPresentation();

			bool resetDevice = false;

			// Create device if need to.
			if (mpDevice == NULL)
			{			
				CreateD3D9Device();
			}

			// Case device already exists.
			else
			{
				if (mSwapChain != NULL)
				{
					D3DPRESENT_PARAMETERS currentPresentParams;
					HRESULT hr;

					hr = mSwapChain->GetPresentParameters(&currentPresentParams);
					if (FAILED(hr))
					{
						ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
							"GetPresentParameters failed", 
							"D3D9RenderWindow::Acquire");
					}

					// Desired parameters are different then current parameters.
					// Possible scenario is that primary window resized and in the mean while another
					// window attached to this device.
					if (memcmp(&currentPresentParams, &mPresentationParam, sizeof(D3DPRESENT_PARAMETERS)) != 0)
					{
						resetDevice = true;					
					}				
				}

				// Make sure depth stencil is set to valid surface. It is going to be
				// grabbed by the primary window using the GetDepthStencilSurface method.
				if (resetDevice == false)
				{
					mpDevice->SetDepthStencilSurface(mDepthBuffer);
				}

			}

			// Reset device will update all render window resources.
			if (resetDevice)
			{
				Reset();
			}

			// No need to reset -> just acquire resources.
			else
			{
				// Update resources of each window.
				AcquireRenderWindowResources();
			}

			return true;
		}


		bool D3D9Device::Acquire( D3D9RenderWindow* renderWin )
		{
			AcquireRenderWindowResources();
			return true;
		}


		void D3D9Device::NotifyDeviceLost()
		{

		}

		bool D3D9Device::IsAutoDepthStencil() const
		{
			return true;
		}

		void D3D9Device::ClearDeviceStreams()
		{
			D3D9RenderSystem* renderSystem = static_cast<D3D9RenderSystem*>(EngineRoot::GetSingleton().GetRenderSystem());

			// Set all texture units to nothing to release texture surfaces
			for (DWORD stage = 0; stage < mD3D9DeviceCaps.MaxSimultaneousTextures; ++stage)
			{
				DWORD   dwCurValue = D3DTOP_FORCE_DWORD;
				HRESULT hr;

				hr = mpDevice->SetTexture(stage, NULL);
				if( hr != S_OK )
				{
					String str = "Unable to disable texture '" + ToString((unsigned int)stage) + "' in D3D9";
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, str, "D3D9Device::ClearDeviceStreams" );
				}

				mpDevice->GetTextureStageState(static_cast<DWORD>(stage), D3DTSS_COLOROP, &dwCurValue);

				if (dwCurValue != D3DTOP_DISABLE)
				{
					hr = mpDevice->SetTextureStageState(static_cast<DWORD>(stage), D3DTSS_COLOROP, D3DTOP_DISABLE);
					if( hr != S_OK )
					{
						String str = "Unable to disable texture '" + ToString((unsigned)stage) + "' in D3D9";
						ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, str, "D3D9Device::ClearDeviceStreams" );
					}
				}			


				//// set stage desc. to defaults
				//renderSystem->mTexStageDesc[stage].pTex = 0;
				//renderSystem->mTexStageDesc[stage].autoTexCoordType = TEXCALC_NONE;
				//renderSystem->mTexStageDesc[stage].coordIndex = 0;
				//renderSystem->mTexStageDesc[stage].texType = D3D9Mappings::D3D_TEX_TYPE_NORMAL;
			}

			// Unbind any vertex streams to avoid memory leaks				
			for (unsigned int i = 0; i < mD3D9DeviceCaps.MaxStreams; ++i)
			{
				mpDevice->SetStreamSource(i, NULL, 0, 0);
			}
		}

		void D3D9Device::Present( D3D9RenderWindow* renderWindow )
		{
			
			// Skip present while current device state is invalid.
			if (mDeviceLost || mAcquired == false || IsDeviceLost())		
				return;		

			HRESULT hr;

			hr = mSwapChain->Present(NULL, NULL, NULL, NULL, 0);			
		
			if( D3DERR_DEVICELOST == hr)
			{
				ReleaseRenderWindowResource();
				NotifyDeviceLost();
			}
			else if( FAILED(hr) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Error Presenting Surfaces", 
					"D3D9Device::Present" );
			}
			else
			{
				//mLastPresentFrame = EngineRoot::GetSingleton().GetNextFrameNumber();
			}
		}
	}
}