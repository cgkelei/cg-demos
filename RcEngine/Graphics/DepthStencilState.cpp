#include <Graphics/DepthStencilState.h>

namespace RcEngine {

DepthStencilStateDesc::DepthStencilStateDesc()
	: DepthEnable(true), 
	  DepthWriteMask(true), 
	  DepthFunc(CF_Less),
	  StencilEnable(false),
	  StencilReadMask(DEFAULT_STENCIL_READ_MASK), 
	  StencilWriteMask(DEFAULT_STENCIL_WRITE_MASK), 
	  FrontStencilFunc(CF_Always),
	  FrontStencilFailOp(SOP_Keep), 
	  FrontStencilDepthFailOp(SOP_Keep), 
	  FrontStencilPassOp(SOP_Keep), 
	  BackStencilFunc(CF_Always),
	  BackStencilFailOp(SOP_Keep), 
	  BackStencilDepthFailOp(SOP_Keep), 
	  BackStencilPassOp(SOP_Keep)
{

}

bool operator< (const DepthStencilStateDesc& lhs, const DepthStencilStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}


}