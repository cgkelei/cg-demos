#include "D3D9RenderSystem.h"
#include "D3D9RenderWindow.h"
#include "D3D9DriverList.h"
#include "D3D9Device.h"
#include "D3D9ResourceManager.h"
#include "D3D9DeviceManager.h"
#include "D3D9RenderStateManager.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9Mapping.h"
#include "LogManager.h"
#include "ContextCfg.h"
#include "Exception.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{


		D3D9RenderSystem* D3D9RenderSystem::msD3D9RenderSystem  = NULL;

		D3D9RenderSystem::D3D9RenderSystem(HINSTANCE hInst)
			: RenderSystem(),
			  mpD3D(NULL),
			  mDriverList(NULL),
			  mDeviceManager(NULL),
			  mResourceManager(NULL),
			  mhInstance(hInst),
			  mActiveD3DDriver(NULL),
			  mFSAAHint(""),
			  mFSAASamples(0),
			  mLastVertexSourceCount(0)
				
		{
			// update singleton access pointer.
			msD3D9RenderSystem = this;
			

			// Create our Direct3D object
			if( NULL == (mpD3D = Direct3DCreate9(D3D_SDK_VERSION)) )
				ENGINE_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Failed to create Direct3D9 object", "D3D9RenderSystem::D3D9RenderSystem" );
			
			mRenderStateManager = new D3D9RenderStateManager();

			mEventNames.push_back("DeviceLost");
			mEventNames.push_back("DeviceRestored");

		}

		D3D9RenderSystem::~D3D9RenderSystem(void)
		{
			Shutdown();

			SAFE_RELEASE( mpD3D );

			msD3D9RenderSystem = NULL;
		}

		const String& D3D9RenderSystem::GetName() const
		{
			static String strName( "Direct3D9 Rendering Subsystem");
			return strName;
		}
		
		D3D9DriverList* D3D9RenderSystem::GetDirect3DDrivers()
		{
			if( !mDriverList )
				mDriverList = new D3D9DriverList();

			return mDriverList;
		}

		RenderWindow* D3D9RenderSystem::CreateRenderWindow(const String &name, const RenderSettings& settings)
		{		
			LogMessageStream << "D3D9RenderSystem:: CreateRenderWindow \"" << name << "\", " <<
				settings.Width << "x" << settings.Height << " ";
			if(settings.Fullscreen)
				LogMessageStream << "fullscreen ";
			else
				LogMessageStream << "windowed ";


			String msg;

			mRenderSettings = settings;

			// Make sure we don't already have a render target of the 
			// same name as the one supplied
			if( mRenderTargets.find( name ) != mRenderTargets.end() )
			{
				msg = "A render target of the same name '" + name + "' already "
					"exists.  You cannot create a new window with this name.";
				ENGINE_EXCEPT( Exception::ERR_INTERNAL_ERROR, msg, "D3D9RenderSystem::CreateRenderWindow" );
			}

			D3D9RenderWindow* renderWindow = new D3D9RenderWindow(mhInstance, name, mRenderSettings);
			renderWindow->Initialize();

			mRenderWindows.push_back(renderWindow);		

			mDeviceManager->LinkRenderWindow(renderWindow);

			//UpdateRenderSystemCapabilities(renderWindow);

			AttachRenderTarget( *renderWindow );

			return renderWindow;
		}



		RenderWindow* D3D9RenderSystem::Initialize( bool autoCreateWindow, const String& windowTitle /*= "Render Window"*/ )
		{	
			RenderWindow* autoWindow = NULL;
			LogMessageStream<<  "D3D9 : Subsystem Initialising" << std::endl;

			// Init using current settings
			mActiveD3DDriver = NULL;

			/*if( !mActiveD3DDriver )
				ENGINE_EXCEPT( Exception::ERR_INVALIDPARAMS, "Problems finding requested Direct3D driver!", "D3D9RenderSystem::initialise" );*/

			//// get driver version
			//mDriverVersion.major = HIWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.HighPart);
			//mDriverVersion.minor = LOWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.HighPart);
			//mDriverVersion.release = HIWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.LowPart);
			//mDriverVersion.build = LOWORD(mActiveD3DDriver->getAdapterIdentifier().DriverVersion.LowPart);

			// Create the device manager.
			mDeviceManager = new D3D9DeviceManager();

			//// Create the texture manager for use by others		
			//mTextureManager = new D3D9TextureManager();

			//// Also create hardware buffer manager		
			//mHardwareBufferManager = new D3D9HardwareBufferManager();

			//// Create the GPU program manager		
			//mGpuProgramManager = new D3D9GpuProgramManager();

			//// Create & register HLSL factory		
			//mHLSLProgramFactory = new D3D9HLSLProgramFactory();

			if( autoCreateWindow )
			{

				autoWindow = CreateRenderWindow( windowTitle, ContextCfg::GetSingleton().GetRenderSettings());
			}	

			LogMessageStream << "***************************************" << std::endl;
			LogMessageStream <<"*** D3D9 : Subsystem Initialized OK ***"<< std::endl;
			LogMessageStream <<"***************************************"<< std::endl;

			// call superclass method
			RenderSystem::Initialize(autoCreateWindow, windowTitle);


			return autoWindow;
		}


		void D3D9RenderSystem::Shutdown()
		{
			RenderSystem::Shutdown();
		}

		IDirect3D9* D3D9RenderSystem::GetDirect3D9()
		{
			IDirect3D9* pDirect3D9 = msD3D9RenderSystem->mpD3D;

			if (pDirect3D9 == NULL)
			{
				ENGINE_EXCEPT( Exception::ERR_INVALIDPARAMS, 
					"Direct3D9 interface is NULL !!!", 
					"D3D9RenderSystem::getDirect3D9" );
			}

			return pDirect3D9;
		}

		void D3D9RenderSystem::SetAmbientLight( float r, float g, float b )
		{
			HRESULT hr = SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( r, g, b, 1.0f ) );
			if( FAILED( hr ) )
				ENGINE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, 
				"Failed to set render stat D3DRS_AMBIENT", "D3D9RenderSystem::SetAmbientLight" );
		}

		void D3D9RenderSystem::SetShadingType( ShadingType st )
		{
			HRESULT hr = SetRenderState( D3DRS_SHADEMODE, D3D9Mapping::Mapping(st) );
			if( FAILED( hr ) )
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
				"Failed to set render stat D3DRS_SHADEMODE", "D3D9RenderSystem::SetShadingType" );
		}

		void D3D9RenderSystem::BeignFrame()
		{
			HRESULT hr;

			if( !mActiveViewport )
				ENGINE_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Cannot begin frame - no viewport selected.", "D3D9RenderSystem::BeginFrame" );

			if( FAILED( hr = GetActiveD3D9Device()->BeginScene() ) )
			{
				String msg = DXGetErrorDescription(hr);
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error beginning frame :" + msg, "D3D9RenderSystem::BeginFrame" );
			}

			mLastVertexSourceCount = 0;

			// Clear left overs of previous viewport.
			// I.E: Viewport A can use 3 different textures and light states
			// When trying to render viewport B these settings should be cleared, otherwise 
			// graphical artifacts might occur.
			mDeviceManager->GetActiveDevice()->ClearDeviceStreams();
		}
		void D3D9RenderSystem::EndFrame()
		{

		}

		void D3D9RenderSystem::SetProjectionMatrix( const Matrix4f& m )
		{

		}

		//////////////////////////////////////////////////////////////////////////
		HRESULT D3D9RenderSystem::SetRenderState( D3DRENDERSTATETYPE state, DWORD value )
		{
			HRESULT hr;
			DWORD oldVal;

			if ( FAILED( hr = GetActiveD3D9Device()->GetRenderState(state, &oldVal) ) )
				return hr;
			if ( oldVal == value )
				return D3D_OK;
			else
				return GetActiveD3D9Device()->SetRenderState(state, value);
		}

		D3D9ResourceManager* D3D9RenderSystem::GetResourceManager()
		{
			return msD3D9RenderSystem->mResourceManager;
		}

		D3D9DeviceManager* D3D9RenderSystem::GetDeviceManager()
		{
			return msD3D9RenderSystem->mDeviceManager;
		}

		IDirect3DDevice9* D3D9RenderSystem::GetActiveD3D9Device()
		{
			D3D9Device* activeDevice = msD3D9RenderSystem->mDeviceManager->GetActiveDevice();
			IDirect3DDevice9* d3d9Device;

			d3d9Device = activeDevice->GetD3D9Device();

			if (d3d9Device == NULL)
			{
				ENGINE_EXCEPT( Exception::ERR_INVALIDPARAMS, 
					"Current d3d9 device is NULL !!!", 
					"D3D9RenderSystem::GetActiveD3D9Device" );
			}

			return d3d9Device;
		}

		void D3D9RenderSystem::Reinitialize()
		{
			LogMessageStream << "D3D9 : Reinitializing" << std::endl;
			this->Shutdown();
			this->Initialize( true );
		}

		void D3D9RenderSystem::SetViewport( Viewport *vp )
		{
			
		}

		void D3D9RenderSystem::SetViewMatrix( const Matrix4f& m )
		{

		}

		void D3D9RenderSystem::SetWorldMatrix( const Matrix4f& m )
		{

		}

		void D3D9RenderSystem::Clear( uint32 buffers /*= CO_Target | CO_DepthBuffer*/, const ColorRGBA& colour /*= Color::Black*/, float depth /*= 1.0f*/, uint16 stencil /*= 0*/ )
		{
			DWORD flags = 0;
			if (buffers & CO_Target)
			{
				flags |= D3DCLEAR_TARGET;
			}
			if (buffers & CO_DepthBuffer)
			{
				flags |= D3DCLEAR_ZBUFFER;
			}
			// Only try to clear the stencil buffer if supported
			if (buffers & CO_Stencil )// && mCurrentCapabilities->hasCapability(RSC_HWSTENCIL))
			{
				flags |= D3DCLEAR_STENCIL;
			}
			HRESULT hr;

			DWORD dwClearColor = D3DCOLOR_COLORVALUE(colour.R(), colour.G(),colour.B(),colour.A());

			if( FAILED( hr = GetActiveD3D9Device()->Clear(0, NULL, flags,
				dwClearColor, depth, stencil ) ) )
			{
				String msg = DXGetErrorDescription(hr);
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error clearing frame buffer : " 
					+ msg, "D3D9RenderSystem::ClearFrameBuffer" );
			}
		}

		RenderStateManager* D3D9RenderSystem::GetRenderStateManager()
		{
			return mRenderStateManager;
		}

		void D3D9RenderSystem::SetVertexDeclaration( VertexDeclaration* decl )
		{
			HRESULT hr;

			D3D9VertexDeclaration* d3ddecl = 
				static_cast<D3D9VertexDeclaration*>(decl);

			if (FAILED(hr = GetActiveD3D9Device()->SetVertexDeclaration(d3ddecl->GetD3DVertexDeclaration())))
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Unable to set D3D9 vertex declaration", 
					"D3D9RenderSystem::setVertexDeclaration");
			}
		}
}


}