#include "D3D9RenderWindow.h"
#include "D3D9RenderSystem.h"
#include "D3D9Device.h"
#include "D3D9DeviceManager.h"
#include "RenderSettings.h"
#include "LogManager.h"
#include "D3D9Mapping.h"
#include "WindowEventUtils.h"


namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		D3D9RenderWindow::D3D9RenderWindow(HINSTANCE hInst, const String& name, const RenderSettings& settings)
			: RenderWindow(),
			  mInstance(hInst),
			  mDevice( NULL ),
			  mHWnd(0),
			  mClosed(false),
			  mSwitchingFullscreen(false),
			  mDisplayFrequency(0),
			  mDeviceValid(false)
		{
			  mName = name;
			  mActive = false;

			  // 强制屏幕刷新率
			  mDisplayFrequency = 60;
			
			  mIsFullScreen = settings.Fullscreen;

			  mLeft = settings.Left;
			  mTop = settings.Top;
			  mWidth = settings.Width;
			  mHeight = settings.Height;
			  
			  // Get Depth Stencil Format Bits
			  PixelFormatUtils::GetNumDepthStencilBits(settings.DepthStencilForamt, mDepthBits, mStencilBits);
				
			  if(mDepthBits > 0)
				mIsDepthBuffered = true;
			  else
				mIsDepthBuffered = false;

			  mColorFormat = settings.ColorFormat;
			  mColourDepth = PixelFormatUtils::GetNumElemBits(settings.ColorFormat);
			  

			  mMutilSampleCount = settings.SampleCount;
			  mMutilSampleQuality = settings.SampleQuality;

			  mVSync = (settings.SyncInterval > 0) ? true : false;
			  mVSyncInterval = settings.SyncInterval;	 
			  
		}

		D3D9RenderWindow::~D3D9RenderWindow()
		{
			Destroy();
		}
	
		void D3D9RenderWindow::Initialize()
		{		
			// Destroy current window if any
			if( mHWnd )
				Destroy();

			DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
			DWORD dwStyleEx = 0;

			if (mIsFullScreen)
			{
				dwStyleEx |= WS_EX_TOPMOST;
				dwStyle |= WS_POPUP;
				mTop = 0;
				mLeft =0;		
			}else
			{		
				dwStyle |= WS_OVERLAPPEDWINDOW;

			}

			uint32 winWidth, winHeight;
			AdjustWindow(mWidth, mHeight, dwStyle, &winWidth, &winHeight);


			WNDCLASS wc = { 0, WindowEventUtilities::WndProc, 0, 0, mInstance,
				LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "ENGINED3D9Wnd" };
			RegisterClass(&wc);

			// Create our main window
			// Pass pointer to self
			mHWnd = CreateWindowEx(dwStyleEx, "ENGINED3D9Wnd", mName.c_str(), dwStyle,
				mLeft, mTop, winWidth, winHeight, 0, 0, mInstance, this);

			LogMessageStream << "D3D9 : Created D3D9 Rendering Window '"
				<< mName << "' : " << mWidth << "x" << mHeight 
				<< ", " << mColourDepth << "bpp" << std::endl;

			mActive = true;
			mClosed = false;
			mStyle = dwStyle;

			Engine::WindowEventUtilities::AddRenderWindow(this);

		}

		void D3D9RenderWindow::BeginUpdate()
		{
	
			if (mWidth == 0 || mHeight == 0)
			{
				mDeviceValid = false;
				return;
			}

			D3D9RenderSystem::GetDeviceManager()->SetActiveRenderTargetDevice(mDevice);

			// Check that device can be used for rendering operations.
			mDeviceValid = mDevice->Validate(this);
			if (mDeviceValid)
			{
				// Finish window / fullscreen mode switch.
				if (GetSwitchingFullscreen())
				{
					FinishSwitchingFullscreen();		
					// have to re-validate since this may have altered dimensions
					mDeviceValid = mDevice->Validate(this);
				}
			}

			RenderWindow::BeginUpdate();
		}

	
		void D3D9RenderWindow::EndUpdate()
		{

		}


		bool D3D9RenderWindow::IsActive() const
		{
			if(IsFullscreen())
				return IsVisible();

			return mActive && IsVisible();
		}

	
		void D3D9RenderWindow::AdjustWindow(uint32 clientWidth, uint32 clientHeight, DWORD style, uint32* winWidth, uint32* winHeight)
		{
			// only call this for non full screen
			RECT rc;
			SetRect(&rc, 0, 0, clientWidth, clientHeight);
			AdjustWindowRect(&rc, style, false);
			*winWidth = rc.right - rc.left;
			*winHeight = rc.bottom - rc.top;
		}

		void D3D9RenderWindow::Destroy(void)
		{
			if (mDevice != NULL)
			{
				mDevice->DetachRenderWindow(this);
				mDevice = NULL;
			}

			if (mHWnd)
			{
				WindowEventUtilities::RemoveRenderWindow(this);
				DestroyWindow(mHWnd);
			}

			mHWnd = 0;
			mActive = false;
			mClosed = true;
		}

		void D3D9RenderWindow::Resize(uint32 width, uint32 height)
		{
			if (mHWnd && !mIsFullScreen)
			{
				mWidth = width;
				mHeight = height;

				unsigned int winWidth, winHeight;
				AdjustWindow(width, height, mStyle, &winWidth, &winHeight);
				// Retain Left, Top Coordinate
				SetWindowPos(mHWnd, 0, 0, 0, winWidth, winHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}

		void D3D9RenderWindow::Reposition(uint32 left, uint32 top)
		{
			if (mHWnd && !mIsFullScreen)
			{
				mLeft = left;
				mTop = top;
				SetWindowPos(mHWnd, 0, top, left, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}


		void D3D9RenderWindow::WindowMovedOrResized()
		{
			if (!mHWnd || IsIconic(mHWnd))
				return;

			UpdateWindowRect();
		}

		void D3D9RenderWindow::SetFullscreen(bool fullScreen)
		{	
			if( mIsFullScreen != fullScreen)
			{
				mSwitchingFullscreen = true;
				mStyle = WS_VISIBLE | WS_CLIPCHILDREN;


				mIsFullScreen = fullScreen;
				
				if (fullScreen)
				{
					mStyle |= WS_POPUP;

					mTop = 0;
					mLeft = 0;

					// Change from window to fullscreen
					SetWindowPos(mHWnd, HWND_TOPMOST, mLeft, mTop, mWidth, mHeight, SWP_NOACTIVATE);
					SetWindowLong(mHWnd, GWL_STYLE, mStyle);
					SetWindowPos(mHWnd, 0, 0,0, 0,0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				}else
				{
					mStyle |= WS_OVERLAPPEDWINDOW;
					unsigned int winWidth, winHeight;
					AdjustWindow(mWidth, mHeight, mStyle, &winWidth, &winHeight);

					SetWindowLong(mHWnd, GWL_STYLE, mStyle);
					SetWindowPos(mHWnd, HWND_NOTOPMOST, mLeft, mTop, winWidth, winHeight,
						SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOACTIVATE);
				}

			}
		}

		void D3D9RenderWindow::FinishSwitchingFullscreen()
		{
			if(mIsFullScreen)
			{
				// Need to reset the region on the window sometimes, when the 
				// windowed mode was constrained by desktop 
				HRGN hRgn = CreateRectRgn(0, 0, mWidth, mHeight);
				SetWindowRgn(mHWnd, hRgn, FALSE);
			}else
			{
				// When switching back to windowed mode, need to reset window size 
				// after device has been restored
				// We may have had a resize event which polluted our desired sizes
				unsigned int winWidth, winHeight;
				AdjustWindow(mWidth, mHeight, mStyle, &winWidth, &winHeight);

				// deal with centreing when switching down to smaller resolution
				uint32 screenw,screenh;
				screenw = GetSystemMetrics(SM_CXSCREEN);
				screenh = GetSystemMetrics(SM_CYSCREEN);

				int left = screenw > winWidth ? ((screenw - winWidth) / 2) : 0;
				int top = screenh > winHeight ? ((screenh - winHeight) / 2) : 0;

				SetWindowPos(mHWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
					SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);
			}
			mSwitchingFullscreen = false;
			
		}


		void D3D9RenderWindow::UpdateWindowRect()
		{
			RECT rc;

			// Update top left parameters
			assert( GetWindowRect(mHWnd, &rc) );
			
			mTop = rc.top;
			mLeft = rc.left;
			
			// width and height represent drawable area only
			assert( GetClientRect(mHWnd, &rc) );
			unsigned int width = rc.right - rc.left;
			unsigned int height = rc.bottom - rc.top;

			// Case window resized.
			if (width != mWidth || height != mHeight)
			{
				mWidth  = rc.right - rc.left;
				mHeight = rc.bottom - rc.top;

				// Notify viewports of resize
				mViewport->UpdateDimensions();
			}	
		}

		
		void D3D9RenderWindow::SwapBuffers( bool waitForVSync )
		{
			if(mDeviceValid)
			{
				mDevice->Present(this);
			}
		}

		void D3D9RenderWindow::BuildPresentParameters(D3DPRESENT_PARAMETERS* presentParams)
		{
			// Set up the presentation parameters		
			IDirect3D9* pD3D = D3D9RenderSystem::GetDirect3D9();
			D3DDEVTYPE devType = D3DDEVTYPE_HAL;

			if (mDevice != NULL)		
				devType = mDevice->GetDeviceType();		

			ZeroMemory( presentParams, sizeof(D3DPRESENT_PARAMETERS) );
			presentParams->Windowed					= !mIsFullScreen;
			presentParams->SwapEffect				= D3DSWAPEFFECT_DISCARD;
			// triple buffer if VSync is on
			presentParams->BackBufferCount			= mVSync ? 2 : 1;
			presentParams->EnableAutoDepthStencil	= mIsDepthBuffered;
			presentParams->hDeviceWindow			= mHWnd;
			presentParams->BackBufferWidth			= mWidth;
			presentParams->BackBufferHeight			= mHeight;
			presentParams->FullScreen_RefreshRateInHz = mIsFullScreen ? mDisplayFrequency : 0;

			if (presentParams->BackBufferWidth == 0)		
				presentParams->BackBufferWidth = 1;					

			if (presentParams->BackBufferHeight == 0)	
				presentParams->BackBufferHeight = 1;

			// 设置垂直刷新率

			if (mVSync)
			{
				if(mIsFullScreen)
				{
					switch(mVSyncInterval)
					{
					case 1:
					default:
						presentParams->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
						break;
					case 2:
						presentParams->PresentationInterval = D3DPRESENT_INTERVAL_TWO;
						break;
					case 3:
						presentParams->PresentationInterval = D3DPRESENT_INTERVAL_THREE;
						break;
					case 4:
						presentParams->PresentationInterval = D3DPRESENT_INTERVAL_FOUR;
						break;
					};
					// check that the interval was supported, revert to 1 to be safe otherwise
					D3DCAPS9 caps;
					pD3D->GetDeviceCaps(mDevice->GetAdapterNumber(), devType, &caps);
					if (!(caps.PresentationIntervals & presentParams->PresentationInterval))
						presentParams->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
				}
				else
				{
					presentParams->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
				}
			}
			else
			{
				presentParams->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			}

			// 设置后背缓冲格式，后续需要修改以支持Aplha
			
			presentParams->BackBufferFormat	= D3DFMT_R5G6B5;
			if( mColourDepth > 16 )
				presentParams->BackBufferFormat = D3DFMT_X8R8G8B8;


			// 设置深度缓存，蒙板缓存
			if( mIsDepthBuffered)
			{
				assert((32 == mDepthBits) || (24 == mDepthBits) || (16 == mDepthBits) || (0 == mDepthBits));
				assert((8 == mStencilBits) || (0 == mStencilBits));


				if( 32 == mDepthBits )
				{
					assert(0 == mStencilBits);
					if(SUCCEEDED( pD3D->CheckDeviceFormat(mDevice->GetAdapterNumber(),
						devType,  presentParams->BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
						D3DRTYPE_SURFACE, D3DFMT_D32 )))

					{
						presentParams->AutoDepthStencilFormat = D3DFMT_D32;
						
					}else
					{
						// Failed, Try 24 bits depth
						mDepthBits = 24;
					}
				}

				if( 24 == mDepthBits )
				{
					// No matter set stencil or not, just try it 
					if( SUCCEEDED( pD3D->CheckDeviceFormat(mDevice->GetAdapterNumber(),
							devType,  presentParams->BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
							D3DRTYPE_SURFACE, D3DFMT_D24S8 )))
					{
						presentParams->AutoDepthStencilFormat = D3DFMT_D24S8;
						mStencilBits = 8;
					}
					else if( SUCCEEDED( pD3D->CheckDeviceFormat(mDevice->GetAdapterNumber(),
						devType,  presentParams->BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
						D3DRTYPE_SURFACE, D3DFMT_D24X8 )))
					{
						// if can't create 24 bits depth 8 bits stencil, try 24bit depth
						presentParams->AutoDepthStencilFormat = D3DFMT_D24X8;
						mStencilBits = 0;
						
					}
					else
					{
						mDepthBits = 16;
					}
				
				}

				if( 16 == mDepthBits )
				{
					if( SUCCEEDED( pD3D->CheckDeviceFormat(mDevice->GetAdapterNumber(),
						devType,  presentParams->BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
						D3DRTYPE_SURFACE, D3DFMT_D16 )))
					{
						presentParams->AutoDepthStencilFormat = D3DFMT_D16;
						mStencilBits = 0;
					}
					else
					{
						mDepthBits = 0;
					}
				}

				if( 0 == mDepthBits )
				{
					mIsDepthBuffered = false;
					mStencilBits = 0;
				}
			}
			else
			{
				// Not Set Depth Buffer
				mStencilBits = 0;
				mDepthBits = 0;
			}


			//if (mColourDepth > 16 )
			//{
			//	// Try to create a 24-bit depth, 8-bit stencil
			//	if( FAILED( pD3D->CheckDeviceFormat(mDevice->GetAdapterNumber(),
			//		devType,  presentParams->BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
			//		D3DRTYPE_SURFACE, D3DFMT_D24S8 )))
			//	{
			//		//  no 8-bit hardware stencil, just try 32-bit zbuffer 
			//		if( FAILED( pD3D->CheckDeviceFormat(mDevice->GetAdapterNumber(),
			//			devType,  presentParams->BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
			//			D3DRTYPE_SURFACE, D3DFMT_D32 )))
			//		{
			//			presentParams->AutoDepthStencilFormat = D3DFMT_D16;
			//		}
			//		else
			//			presentParams->AutoDepthStencilFormat = D3DFMT_D32;
			//	}
			//	else
			//	{
			//		if( SUCCEEDED( pD3D->CheckDepthStencilMatch( mDevice->GetAdapterNumber(), devType,
			//			presentParams->BackBufferFormat, presentParams->BackBufferFormat, D3DFMT_D24S8 ) ) )
			//		{
			//			presentParams->AutoDepthStencilFormat = D3DFMT_D24S8; 
			//		} 
			//		else 
			//			presentParams->AutoDepthStencilFormat = D3DFMT_D24X8; 
			//	}
			//}
			//else
			//	// 16-bit depth, software stencil
			//	presentParams->AutoDepthStencilFormat	= D3DFMT_D16;
			
			// 设置多重采样
			DWORD QualityLevelsColor = 0;
			DWORD QualityLevelsDepthStencil = 0;

			if( SUCCEEDED(pD3D->CheckDeviceMultiSampleType( mDevice->GetAdapterNumber(), 
				devType,  presentParams->BackBufferFormat, 
				!mIsFullScreen, D3D9Mapping::Mapping(mMutilSampleCount), &QualityLevelsColor ) ) &&
				SUCCEEDED(pD3D->CheckDeviceMultiSampleType( mDevice->GetAdapterNumber(),
				devType,  presentParams->AutoDepthStencilFormat, 
				!mIsFullScreen, D3D9Mapping::Mapping(mMutilSampleCount), &QualityLevelsDepthStencil ) ) )
			{

				mMutilSampleQuality = min( mMutilSampleQuality, min(QualityLevelsColor, QualityLevelsDepthStencil) );
				presentParams->MultiSampleQuality = mMutilSampleQuality;
				presentParams->MultiSampleType = D3D9Mapping::Mapping(mMutilSampleCount);
			}
			else
			{
				mMutilSampleCount = 0;
				mMutilSampleQuality = 0;
				presentParams->MultiSampleType = D3DMULTISAMPLE_NONE;
				presentParams->MultiSampleQuality = 0;
			}


			// Check sRGB
			if (mHardwareGamma)
			{
				/* hmm, this never succeeds even when device does support??
				if(FAILED(pD3D->CheckDeviceFormat(mDriver->getAdapterNumber(),
				devType, presentParams->BackBufferFormat, D3DUSAGE_QUERY_SRGBWRITE, 
				D3DRTYPE_SURFACE, presentParams->BackBufferFormat )))
				{
				// disable - not supported
				mHwGamma = false;
				}
				*/

			}

		}

		
	}
}