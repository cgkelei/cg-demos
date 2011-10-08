#pragma once
#ifndef _RenderState__H
#define _RenderState__H

#include "Prerequisites.h"
#include "GraphicsCommon.h"
#include "ColorRGBA.h"

namespace Engine
{
	struct _ApiExport RasterizerStateDesc
	{
		PolygonMode		FillMode;
		CullingMode	    CullMode;
		ShadingType     ShadingMode;
		
		// Determines if a triangle is front- or back-facing. If this parameter is true,
		// then a triangle will be considered front-facing if its vertices are counter-clockwise 
		// on the render target and considered back-facing if they are clockwise. If this parameter
		// is false then the opposite is true.

		bool            FrontCounterClockwise;
		
		// The offset is Scale*dZ + Bias*r where dZ is the change in depth
		// relative to the screen space area of the poly, and r is the smallest
		// resolvable depth difference.  Negative values move polygons closer to
		// the eye.
		float			PolygonOffsetScale;  // default: 0
		float		    PolygonOffsetBias;   // default: 0

		bool            DepthClipEnable;
		bool            ScissorEnable;
		bool            MultisampleEnable;

		RasterizerStateDesc();

		friend bool operator< ( const RasterizerStateDesc& lhs, const RasterizerStateDesc& rhs);
	

	};

	// 暂时使用，以后换shared_ptr
	typedef RasterizerStateDesc* RasterizerStatePtr;


	struct _ApiExport DepthStencilStateDesc
	{
		bool                       DepthEnable;
		bool				       DepthWritable;
		CompareFunction            DepthFunc;

		bool                       FrontStencilEnable;
		uint8                      FrontStencilReadMask;
		uint8                      FrontStencilWriteMask;
		StencilOperation		   FrontStencilFailOp;
		StencilOperation           FrontStencilDepthFailOp;
		StencilOperation           FrontStencilPassOp;
		CompareFunction            FrontStencilFunc;

		bool                       BackStencilEnable;
		uint8                      BackStencilReadMask;
		uint8                      BackStencilWriteMask;
		StencilOperation		   BackStencilFailOp;
		StencilOperation           BackStencilDepthFailOp;
		StencilOperation           BackStencilPassOp;
		CompareFunction            BackStencilFunc;
		
		DepthStencilStateDesc();

		friend bool operator< ( const DepthStencilStateDesc& lhs, const DepthStencilStateDesc& rhs);

	};

	// 暂时使用，以后换shared_ptr
	typedef DepthStencilStateDesc* DepthStencilStatePtr;

	struct _ApiExport BlendStateDesc
	{
		// Determines whether or not to use alpha-to-coverage as a multisampling technique when setting a pixel to a rendertarget
		bool                           AlphaToCoverageEnable;

		// Set to true to enable independent blending in simultaneous render targets.
		// If set to false, only the RenderTarget[0] members are used. RenderTarget[1..7] are ignored.
		bool                           IndependentBlendEnable;

			
		struct RenderTargetBlendDesc
		{
			bool           BlendEnable;
			BlendFactor    SrcBlend;
			BlendFactor    DestBlend;
			BlendFunction  BlendOp;

			BlendFactor    SrcBlendAlpha;
			BlendFactor    DestBlendAlpha;
			BlendFunction  BlendOpAlpha;

			uint8          ColorWriteMask;

		};

		RenderTargetBlendDesc RenderTarget[8];

		BlendStateDesc();
		friend bool operator< ( const BlendStateDesc& lhs, const BlendStateDesc& rhs);

	};
	// 暂时使用，以后换shared_ptr
	typedef BlendStateDesc* BlendStatePtr;

	struct _ApiExport SampleStateDesc
	{
		ColorRGBA				   BorderColor;
		
		TextureAddressMode		   AddressU;
		TextureAddressMode		   AddressV;
		TextureAddressMode		   AddressW;

		FilterOp                   Filter;
		float                      MinLOD;
		float                      MaxLOD;
		float                      MipMapLodBias;
		uint8                      MaxAnisotropy;

		CompareFunction            ComparisonFunc;

		friend bool operator< ( const SampleStateDesc& lhs, const SampleStateDesc& rhs);

	};

	// 暂时使用，以后换shared_ptr
	typedef SampleStateDesc* SampleStatePtr;
}

#endif