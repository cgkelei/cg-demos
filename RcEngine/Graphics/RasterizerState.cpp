#include <Graphics/RasterizerState.h>

namespace RcEngine {

RasterizerStateDesc::RasterizerStateDesc()
	: PolygonFillMode(FM_Solid), 
	  PolygonCullMode(CM_Back), 
	  FrontCounterClockwise(false),
	  DepthBias(0.0f), 
	  SlopeScaledDepthBias(0.0f),
	  DepthClipEnable(true), 
	  ScissorEnable(false), 
	  MultisampleEnable(false)
{

}

bool operator< (const RasterizerStateDesc& lhs, const RasterizerStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

} // Namespace RcEngine

