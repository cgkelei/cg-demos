#ifndef DepthStencilState_h__
#define DepthStencilState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

#define	DEFAULT_STENCIL_READ_MASK	( 0xff )
#define	DEFAULT_STENCIL_WRITE_MASK	( 0xff )

#pragma pack(push, 1)

struct _ApiExport DepthStencilStateDesc
{
	DepthStencilStateDesc();

	bool                       DepthEnable;
	bool					   DepthWriteMask;
	CompareFunction		       DepthFunc;

	bool                       StencilEnable;
	uint8_t                    StencilReadMask;
	uint8_t                    StencilWriteMask;

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

#pragma pack(pop)

class _ApiExport DepthStencilState
{
public:
	DepthStencilState(const DepthStencilStateDesc& desc) 
		: mDesc( desc ) {}
	virtual ~DepthStencilState() {}

	const DepthStencilStateDesc& GetDesc() const					{ return mDesc; } 

protected:
	DepthStencilStateDesc mDesc;
};

} // Namespace RcEngine

#endif // DepthStencilState_h__
