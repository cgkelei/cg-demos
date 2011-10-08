#include "RenderState.h"

namespace Engine
{

	RasterizerStateDesc::RasterizerStateDesc()
		: FillMode(FM_Solid),
		  CullMode(CM_Back),
		  ShadingMode(ST_Gouraud),
		  FrontCounterClockwise(false),
		  PolygonOffsetBias(0),
		  PolygonOffsetScale(0),
		  DepthClipEnable(true),
		  ScissorEnable(false),
		  MultisampleEnable(false)
	{

	}

    bool operator<( const RasterizerStateDesc& lhs, const RasterizerStateDesc& rhs )
	{
		return std::memcmp(&lhs, &rhs, sizeof(RasterizerStateDesc)) < 0;
	}




	DepthStencilStateDesc::DepthStencilStateDesc()
		: DepthEnable(true),
		  DepthWritable(true),
		  DepthFunc(CMPF_Less),
		  FrontStencilEnable(false),
		  FrontStencilReadMask(0xFF),
		  FrontStencilWriteMask(0xFF),
		  FrontStencilFailOp(SOP_Keep),
	      FrontStencilDepthFailOp(SOP_Keep),
	      FrontStencilPassOp(SOP_Keep),
	      FrontStencilFunc(CMPF_Always_Pass),
		  BackStencilEnable(false),
		  BackStencilReadMask(0xFF),
          BackStencilWriteMask(0xFF),
		  BackStencilFailOp(SOP_Keep),
          BackStencilDepthFailOp(SOP_Keep),
	      BackStencilPassOp(SOP_Keep),
	      BackStencilFunc(CMPF_Always_Pass)
	{
		
	}

	bool operator<( const DepthStencilStateDesc& lhs, const DepthStencilStateDesc& rhs )
	{
		return std::memcmp(&lhs, &rhs, sizeof(DepthStencilStateDesc)) < 0;
	}




	BlendStateDesc::BlendStateDesc()
		: AlphaToCoverageEnable(false), 
		IndependentBlendEnable(false)
	{
		RenderTarget[0].BlendEnable = false; 
		RenderTarget[0].SrcBlend = BF_One ;
		RenderTarget[0].DestBlend = BF_Zero ;
		RenderTarget[0].BlendOp = BF_Add ;
		RenderTarget[0].SrcBlendAlpha =BF_One ;
		RenderTarget[0].DestBlendAlpha= BF_Zero ;
		RenderTarget[0].BlendOpAlpha =BF_Add ;
		RenderTarget[0].ColorWriteMask = CM_All ;
	}
	
	bool operator<( const BlendStateDesc& lhs, const BlendStateDesc& rhs )
	{
		return std::memcmp(&lhs, &rhs, sizeof(BlendStateDesc)) < 0;
	}

}