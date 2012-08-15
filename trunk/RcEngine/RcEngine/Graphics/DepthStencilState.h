#ifndef DepthStencilState_h__
#define DepthStencilState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {
namespace Render {

class _ApiExport DepthStencilStateDesc
{
public:
	DepthStencilStateDesc();

	bool                       DepthEnable;
	bool					   DepthWriteMask;
	CompareFunction		       DepthFunc;

	bool                       StencilEnable;
	uint16_t                   StencilReadMask;
	uint16_t                   StencilWriteMask;

	StencilOperation		   FrontStencilFailOp;
	StencilOperation		   FrontStencilDepthFailOp;
	StencilOperation		   FrontStencilPassOp;
	CompareFunction			   FrontStencilFunc;

	StencilOperation		   BackStencilFailOp;
	StencilOperation		   BackStencilDepthFailOp;
	StencilOperation		   BackStencilPassOp;
	CompareFunction			   BackStencilFunc;
};

inline DepthStencilStateDesc::DepthStencilStateDesc()
	: DepthEnable(true), DepthWriteMask(true), DepthFunc(CF_Less),
	StencilEnable(true),StencilReadMask(0xFFFF), StencilWriteMask(0xFFFF), 
	FrontStencilFailOp(SOP_Keep), FrontStencilDepthFailOp(SOP_Keep), FrontStencilPassOp(SOP_Keep), FrontStencilFunc(CF_AlwaysPass),
	BackStencilFailOp(SOP_Keep), BackStencilDepthFailOp(SOP_Keep), BackStencilPassOp(SOP_Keep), BackStencilFunc(CF_AlwaysPass)
{

}

class _ApiExport DepthStencilState
{
public:
	DepthStencilState(const DepthStencilStateDesc& desc) 
		: mDesc( desc )
	{

	}

	const DepthStencilStateDesc& GetDesc() const					{ return mDesc; } 

protected:
	DepthStencilStateDesc mDesc;
};


} // Namespace Render
} // Namespace RcEngine

#endif // DepthStencilState_h__
