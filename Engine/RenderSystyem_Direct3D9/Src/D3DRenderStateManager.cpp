#include "D3D9RenderStateManager.h"
#include "D3D9RenderSystem.h"
#include "D3D9Mapping.h"
#include "Exception.h"

#define FLOAT2DWORD(f) *((DWORD*)&f)
#define DWORD2FLOAT(d) *((float*)&d)


namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
	
		D3D9RenderStateManager::D3D9RenderStateManager()
			: RenderStateManager()
		{

		}

		D3D9RenderStateManager::~D3D9RenderStateManager()
		{

		}
		
		//////////////////////////////////////////////////////////////////////////
		HRESULT D3D9RenderStateManager::GetRenderState(D3DRENDERSTATETYPE state, DWORD& value) const 
		{
			return D3D9RenderSystem::GetActiveD3D9Device()->GetRenderState(state, &value);
		}

		HRESULT D3D9RenderStateManager::SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
		{
			HRESULT hr;
			DWORD oldVal;
			
			if( FAILED( hr = GetRenderState(state, oldVal) ) )
				return hr;
			if ( oldVal == value )
				return D3D_OK;
			else
				return D3D9RenderSystem::GetActiveD3D9Device()->SetRenderState(state, value);	
		}

		//////////////////////////////////////////////////////////////////////////
		CullingMode D3D9RenderStateManager::GetCullMode() const
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_CULLMODE, oldVal) ))
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get cull mode, Maybe device not right",
					"D3D9RenderStateManager::GetCullMode");
			}
			
			return D3D9Mapping::UnMapping(static_cast<D3DCULL>(oldVal));
				
		}

		void D3D9RenderStateManager::SetCullMode(CullingMode cm)
		{
			HRESULT hr = SetRenderState(D3DRS_CULLMODE, D3D9Mapping::Mapping(cm));
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error Set CullMode", 
				"D3D9RenderSystem::SetCullMode");

		}
		
		//////////////////////////////////////////////////////////////////////////
		PolygonMode D3D9RenderStateManager::GetFillMode() const
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_FILLMODE, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get fill mode, Maybe device not right",
					"D3D9RenderStateManager::GetCullMode");
			}

			return D3D9Mapping::UnMapping(static_cast<D3DFILLMODE>(oldVal));
		}

		void D3D9RenderStateManager::SetFillMode(PolygonMode fm)
		{
			HRESULT hr = SetRenderState(D3DRS_FILLMODE, D3D9Mapping::Mapping(fm));
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error Set Fill Mode", 
				"D3D9RenderSystem::SetFillMode");

		}

		//////////////////////////////////////////////////////////////////////////
		bool D3D9RenderStateManager::IsDepthBufferEnabled() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_ZENABLE, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get ZEnabled, Maybe device not right",
					"D3D9RenderStateManager::IsDepthBufferEnabled");
			}

			return oldVal == D3DZB_TRUE;
		}

		bool D3D9RenderStateManager::IsDepthBufferWriteEnabled() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_ZWRITEENABLE , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get ZEnabled Write, Maybe device not right",
					"D3D9RenderStateManager::IsDepthBufferWriteEnabled");
			}

			return oldVal == static_cast<DWORD>(true);
		}

		float D3D9RenderStateManager::GetDepthBias() const
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_DEPTHBIAS, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error Get Depth Bits, Maybe device not right",
					"D3D9RenderStateManager::GetDepthBias");
			} 

			return DWORD2FLOAT(oldVal);
		}

		float D3D9RenderStateManager::GetSlopeScaleDepthBias() const  
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error Get Depth Bits, Maybe device not right",
					"D3D9RenderStateManager::GetDepthBias");
			} 

			return DWORD2FLOAT(oldVal);
		}

		CompareFunction D3D9RenderStateManager::GetDepthBufferFunction() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_ZFUNC  , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get ZFunc, Maybe device not right",
					"D3D9RenderStateManager::GetDepthBufferFunction");
			}

			return D3D9Mapping::UnMapping(static_cast<D3DCMPFUNC>(oldVal));
		}

		//////////////////////////////////////////////////////////////////////////
		void D3D9RenderStateManager::SetDepthBufferCheckEnabled(bool enabled ) 
		{
			HRESULT hr;

			if( enabled )
			{
				hr = SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
			}
			else
				hr = SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

			if( FAILED( hr ) )
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting depth buffer test state", "D3D9RenderSystem::SetDepthBufferCheckEnabled" );
		}

		void D3D9RenderStateManager::SetDepthBufferWriteEnable(bool enabled) 
		{
			HRESULT hr;

			if( FAILED( hr = SetRenderState( D3DRS_ZWRITEENABLE, enabled ) ) )
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting depth buffer write state", "D3D9RenderSystem::SetDepthBufferWriteEnabled" );
		}

		void D3D9RenderStateManager::SetDepthBias(float constantBias, float slopeScaleBias) 
		{
			//后续要根据设备能力修改
			//if ((mDeviceManager->getActiveDevice()->getD3D9DeviceCaps().RasterCaps & D3DPRASTERCAPS_DEPTHBIAS) != 0)
			//{
				// Negate bias since D3D is backward
				// D3D also expresses the constant bias as an absolute value, rather than 
				// relative to minimum depth unit, so scale to fit
				constantBias = -constantBias / 250000.0f;
				HRESULT hr = SetRenderState(D3DRS_DEPTHBIAS, FLOAT2DWORD(constantBias));
				if (FAILED(hr))
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting constant depth bias", 
					"D3D9RenderSystem::SetDepthBias");
			//}

			//if ((mDeviceManager->getActiveDevice()->getD3D9DeviceCaps().RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS) != 0)
			//{
				// Negate bias since D3D is backward
				slopeScaleBias = -slopeScaleBias;
				hr = SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, FLOAT2DWORD(slopeScaleBias));
				if (FAILED(hr))
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting slope scale depth bias", 
					"D3D9RenderSystem::SetDepthBias");
			//}
		}

		void D3D9RenderStateManager::SetDepth(bool depthTest, bool depthWrite ,CompareFunction depthFunction ) 
		{
			SetDepthBufferCheckEnabled(depthTest);
			SetDepthBufferWriteEnable(depthTest);

			HRESULT hr = SetRenderState(D3DRS_ZFUNC, D3D9Mapping::Mapping(depthFunction));
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error SetDepth", 
				"D3D9RenderSystem::SetDepth");
		}

		// Stencil
		bool D3D9RenderStateManager::IsStencilEnabled() 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILENABLE  , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get Stencil Enabled, Maybe device not right",
					"D3D9RenderStateManager::IsStencilEnabled");
			}

			return oldVal == static_cast<DWORD>(true);
		}

		bool D3D9RenderStateManager::IsTwoSidedStencilMode() const 
		{ 
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_TWOSIDEDSTENCILMODE  , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get two side stencil Enabled, Maybe device not right",
					"D3D9RenderStateManager::IsTwoSidedStencilMode");
			}

			return oldVal == static_cast<DWORD>(true);
		}

		StencilOperation D3D9RenderStateManager::GetStencilFail() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILZFAIL , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get stencil fail, Maybe device not right",
					"D3D9RenderStateManager::GetStencilFail");
			}

			return D3D9Mapping::UnMapping(static_cast<D3DSTENCILOP>(oldVal));
		}

		StencilOperation D3D9RenderStateManager::GetStencilPass() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILPASS  , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get stencil pass, Maybe device not right",
					"D3D9RenderStateManager::GetStencilPass");
			}

			return D3D9Mapping::UnMapping(static_cast<D3DSTENCILOP>(oldVal));
		}
		
		StencilOperation D3D9RenderStateManager::GetStencilDepthFail() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILZFAIL  , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get stencil zfail, Maybe device not right",
					"D3D9RenderStateManager::GetStencilDepthFail");
			}

			return D3D9Mapping::UnMapping(static_cast<D3DSTENCILOP>(oldVal));
		}

		CompareFunction  D3D9RenderStateManager::GetStencilFunction() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILFUNC, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get stencil func, Maybe device not right",
					"D3D9RenderStateManager::GetStencilFunction");
			}

			return D3D9Mapping::UnMapping(static_cast<D3DCMPFUNC>(oldVal));
		}

		uint32 D3D9RenderStateManager::GetStencilMask() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILMASK, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get stencil mask, Maybe device not right",
					"D3D9RenderStateManager::GetStencilMask");
			}

			return static_cast<uint32>(oldVal);
		}

		uint32 D3D9RenderStateManager::GetStencilReference() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILREF , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get stencil reference, Maybe device not right",
					"D3D9RenderStateManager::GetStencilReference");
			}

			return static_cast<uint32>(oldVal);
		}

		void D3D9RenderStateManager::SetStencilCheckEnabled(bool enabled) 
		{	 
			HRESULT hr;

			if( FAILED( hr = SetRenderState( D3DRS_STENCILENABLE, enabled ) ) )
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error enabling / disabling stencilling",
				"D3D9RenderSystem::SetStencilCheckEnabled" );
	
		}

		void D3D9RenderStateManager::SetStencil(CompareFunction func /* = CMPF_Always_Pass */, 
			StencilOperation depthFail /*= SOP_Keep  */,
			StencilOperation stencilFail/* = SOP_Keep  */,
			StencilOperation pass /* = SOP_Keep  */,
			uint32 refValue /*=  0 */ , uint32 mask /* = 0xFFFFFFFF  */,
			bool twoSidedOperation/* = false  */) 
		{
			HRESULT hr;
			bool flip;

			// 2-sided operation
			if (twoSidedOperation)
			{
				// 需要根据设备能力检查
			/*	if (!mCurrentCapabilities->hasCapability(RSC_TWO_SIDED_STENCIL))
					OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "2-sided stencils are not supported",
					"D3D9RenderSystem::setStencilBufferParams");
				hr = SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
				if (FAILED(hr))
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting 2-sided stencil mode.",
					"D3D9RenderSystem::SetStencil");

				// NB: We should always treat CCW as front face for consistent with default
				// culling mode. Therefore, we must take care with two-sided stencil settings.
				flip = (mInvertVertexWinding && mActiveRenderTarget->RequiresTextureFlipping()) ||
					(!mInvertVertexWinding && !mActiveRenderTarget->RequiresTextureFlipping());

				// Set alternative versions of ops
				// fail op
				hr = SetRenderState(D3DRS_CCW_STENCILFAIL, D3D9Mappings::get(stencilFailOp, !flip));
				if (FAILED(hr))
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil fail operation (2-sided).",
					"D3D9RenderSystem::setStencilBufferParams");

				// depth fail op
				hr = SetRenderState(D3DRS_CCW_STENCILZFAIL, D3D9Mappings::get(depthFailOp, !flip));
				if (FAILED(hr))
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil depth fail operation (2-sided).",
					"D3D9RenderSystem::setStencilBufferParams");

				// pass op
				hr = SetRenderState(D3DRS_CCW_STENCILPASS, D3D9Mappings::get(passOp, !flip));
				if (FAILED(hr))
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil pass operation (2-sided).",
					"D3D9RenderSystem::setStencilBufferParams");*/
			}
			else
			{
				hr = SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
				if (FAILED(hr))
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting 1-sided stencil mode.",
					"D3D9RenderSystem::SetStencilBufferParams");
				flip = false;
			}

			// func
			hr = SetRenderState(D3DRS_STENCILFUNC, D3D9Mapping::Mapping(func));
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil buffer test function.",
				"D3D9RenderSystem::SetStencil");

			// reference value
			hr = SetRenderState(D3DRS_STENCILREF, refValue);
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil buffer reference value.",
				"D3D9RenderSystem::SetStencil");

			// mask
			hr = SetRenderState(D3DRS_STENCILMASK, mask);
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil buffer mask.",
				"D3D9RenderSystem::SetStencil");

			// fail op
			hr = SetRenderState(D3DRS_STENCILFAIL, D3D9Mapping::Mapping(stencilFail));
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil fail operation.",
				"D3D9RenderSystem::SetStencil");

			// depth fail op
			hr = SetRenderState(D3DRS_STENCILZFAIL, D3D9Mapping::Mapping(depthFail));
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil depth fail operation.",
				"D3D9RenderSystem::SetStencil");

			// pass op
			hr = SetRenderState(D3DRS_STENCILPASS,D3D9Mapping::Mapping(pass));
			if (FAILED(hr))
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error setting stencil pass operation.",
				"D3D9RenderSystem::SetStencil");
		}

		// Aplha Test
		bool D3D9RenderStateManager::IsAlphaTestEnabled() const 
		{
			return true;
		}

		CompareFunction D3D9RenderStateManager::GetAlphaTestFunction() const
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_STENCILFUNC, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get stencil func, Maybe device not right",
					"D3D9RenderStateManager::GetStencilFunction");
			}

			return D3D9Mapping::UnMapping(static_cast<D3DCMPFUNC>(oldVal));
		}

		uint32 D3D9RenderStateManager::GetAlphaReference() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_ALPHAREF, oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get alpha reference , Maybe device not right",
					"D3D9RenderStateManager::GetAlphaReference");
			}

			return static_cast<uint32>(oldVal);
		}

		void D3D9RenderStateManager::SetAlphaTestParams(bool enable, CompareFunction func, uint32 refValue) 
		{

		}

		// Alpha Blend
		bool D3D9RenderStateManager::IsAlphaBlendEnable() const 
		{
			DWORD oldVal;

			if ( FAILED( GetRenderState(D3DRS_ALPHATESTENABLE , oldVal) ) )
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error get alpha test , Maybe device not right",
					"D3D9RenderStateManager::IsAlphaBlendEnable");
			}

			return oldVal == static_cast<DWORD>(true);
		}
	}
}