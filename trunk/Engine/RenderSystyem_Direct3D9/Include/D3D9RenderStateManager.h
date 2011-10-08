#pragma once
#ifndef _D3D9RenderStateManager__H
#define _D3D9RenderStateManager__H

//  [8/24/2011 hustruan]

#include "D3D9Prerequisites.h"
#include "RenderStateManager.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9RenderStateManager : public RenderStateManager
		{
		private:
			D3D9RenderStateManager(const D3D9RenderStateManager& );
			D3D9RenderStateManager& operator= (const D3D9RenderStateManager&);

		public:
			D3D9RenderStateManager();
			~D3D9RenderStateManager();

			// Common
			CullingMode GetCullMode() const;
			void SetCullMode(CullingMode cm);

			PolygonMode GetFillMode() const;
			void SetFillMode(PolygonMode fm);

			// Depth
			bool IsDepthBufferEnabled() const ;
			bool IsDepthBufferWriteEnabled() const ;
			float GetDepthBias() const ;
			float GetSlopeScaleDepthBias() const  ;
			CompareFunction GetDepthBufferFunction() const ;

			void SetDepthBufferCheckEnabled(bool enabled = true) ; 
			void SetDepthBufferWriteEnable(bool enabled = true) ;
			void SetDepthBias(float constantBias, float slopeScaleBias=0.0f) ;
			void SetDepth(bool depthTest = true, bool depthWrite = true,
				CompareFunction depthFunction = CMPF_Less_Equal) ;


			// Stencil
			bool IsStencilEnabled() ;
			bool IsTwoSidedStencilMode() const ;

			StencilOperation GetStencilFail() const ;
			StencilOperation GetStencilPass() const ;
			StencilOperation GetStencilDepthFail() const ;
			CompareFunction  GetStencilFunction() const ;

			uint32 GetStencilMask() const ;
			uint32 GetStencilReference() const ;

			void SetStencilCheckEnabled(bool enabled = true) ;
			void SetStencil(CompareFunction func = CMPF_Always_Pass, 
				StencilOperation depthFail = SOP_Keep,
				StencilOperation stencilFail = SOP_Keep,
				StencilOperation pass = SOP_Keep,
				uint32 refValue = 0 , uint32 mask = 0xFFFFFFFF,
				bool twoSidedOperation = false) ;

			// Aplha Test
			bool IsAlphaTestEnabled() const ;
			CompareFunction GetAlphaTestFunction() const ;
			uint32 GetAlphaReference()const ;

			void SetAlphaTestParams(bool enable, CompareFunction func, uint32 refValue) ;


			// Alpha Blend
			bool IsAlphaBlendEnable() const ;

		private:
			
			HRESULT SetRenderState(D3DRENDERSTATETYPE state, DWORD value);
			HRESULT D3D9RenderStateManager::GetRenderState(D3DRENDERSTATETYPE state, DWORD& value) const;
		};
	}
}




#endif