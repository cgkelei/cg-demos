#ifndef RasterizerState_h__
#define RasterizerState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {


struct _ApiExport RasterizerStateDesc
{
public:
	RasterizerStateDesc();

	FillMode		PolygonFillMode;
	CullMode		PolygonCullMode;
	bool            FrontCounterClockwise;
	float           DepthBias;
	float           SlopeScaledDepthBias;
	bool            DepthClipEnable;
	bool            ScissorEnable;
	bool            MultisampleEnable;

	friend bool operator< (const RasterizerStateDesc& lhs, const RasterizerStateDesc& rhs);
};



class _ApiExport RasterizerState
{
public:
	RasterizerState(const RasterizerStateDesc& desc)
		: mDesc(desc)
	{ }

	const RasterizerStateDesc& GetDesc() const				{ return mDesc; }

protected:
	RasterizerStateDesc mDesc;
};


} // Namespace RcEngine

#endif // RasterizerState_h__
