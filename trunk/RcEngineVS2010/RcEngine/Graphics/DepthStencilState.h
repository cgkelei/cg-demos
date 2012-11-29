#ifndef DepthStencilState_h__
#define DepthStencilState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {


struct _ApiExport DepthStencilStateDesc
{
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
	
    friend bool operator< (const DepthStencilStateDesc& lhs, const DepthStencilStateDesc& rhs);
};


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



} // Namespace RcEngine

#endif // DepthStencilState_h__
