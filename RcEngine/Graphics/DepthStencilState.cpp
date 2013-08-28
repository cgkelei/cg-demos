#include <Graphics/DepthStencilState.h>

namespace RcEngine {

DepthStencilStateDesc::DepthStencilStateDesc()
	: DepthEnable(true), DepthWriteMask(true), DepthFunc(CF_Less),
	StencilEnable(true),StencilReadMask(0xFFFF), StencilWriteMask(0xFFFF), 
	FrontStencilFailOp(SOP_Keep), FrontStencilDepthFailOp(SOP_Keep), FrontStencilPassOp(SOP_Keep), FrontStencilFunc(CF_AlwaysPass),
	BackStencilFailOp(SOP_Keep), BackStencilDepthFailOp(SOP_Keep), BackStencilPassOp(SOP_Keep), BackStencilFunc(CF_AlwaysPass)
{

}

bool operator< (const DepthStencilStateDesc& lhs, const DepthStencilStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}


}