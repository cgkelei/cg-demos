#pragma once
#ifndef _RenderStateManager__H
#define _RenderStateManager__H

#include "Prerequisites.h"
#include "GraphicsCommon.h"

namespace Engine
{
	class _ApiExport RenderStateManager
	{
	private:
		RenderStateManager(const RenderStateManager& );
		RenderStateManager& operator= (const RenderStateManager&);

	public:	
		RenderStateManager();
		virtual ~RenderStateManager();

		// Common
		virtual CullingMode GetCullMode() const = 0;
		virtual void SetCullMode(CullingMode cm) = 0;

		virtual PolygonMode GetFillMode() const = 0;
		virtual void SetFillMode(PolygonMode fm) = 0;

		// Depth
		virtual bool IsDepthBufferEnabled() const = 0;
		virtual bool IsDepthBufferWriteEnabled() const = 0;
		virtual float GetDepthBias() const = 0;
		virtual float GetSlopeScaleDepthBias() const  = 0;
		virtual CompareFunction GetDepthBufferFunction() const = 0;

		virtual void SetDepthBufferCheckEnabled(bool enabled = true) = 0; 
		virtual void SetDepthBufferWriteEnable(bool enabled = true) = 0;
		virtual void SetDepthBias(float constantBias, float slopeScaleBias=0.0f) = 0;
		virtual void SetDepth(bool depthTest = true, bool depthWrite = true,
			CompareFunction depthFunction = CMPF_Less_Equal) = 0;


		// Stencil
		virtual bool IsStencilEnabled() = 0;
		virtual bool IsTwoSidedStencilMode() const = 0;

		virtual StencilOperation GetStencilFail() const = 0;
		virtual StencilOperation GetStencilPass() const = 0;
		virtual StencilOperation GetStencilDepthFail() const = 0;
		virtual CompareFunction  GetStencilFunction() const = 0;

		virtual uint32 GetStencilMask() const = 0;
		virtual uint32 GetStencilReference() const = 0;

		virtual void SetStencilCheckEnabled(bool enabled = true) = 0;
		virtual void SetStencil(CompareFunction func = CMPF_Always_Pass, 
			StencilOperation depthFail = SOP_Keep,
			StencilOperation stencilFail = SOP_Keep,
			StencilOperation pass = SOP_Keep,
			uint32 refValue = 0, uint32 mask = 0xFFFFFFFF,
			bool twoSidedOperation = false) = 0;

		// Aplha Test
		virtual bool IsAlphaTestEnabled() const = 0;
		virtual CompareFunction GetAlphaTestFunction() const = 0;
		virtual uint32 GetAlphaReference()const = 0;
		virtual void SetAlphaTestParams(bool enable, CompareFunction func, uint32 refValue) = 0;

		// Alpha Blend
		virtual bool IsAlphaBlendEnable() const = 0;

	};
}



#endif