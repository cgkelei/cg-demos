#ifndef RasterizerState_h__
#define RasterizerState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {
namespace Render {

class _ApiExport RasterizerStateDesc
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
};

inline RasterizerStateDesc::RasterizerStateDesc()
	: PolygonFillMode(FM_Solid), PolygonCullMode(CM_Back), FrontCounterClockwise(false),
		DepthBias(0.0f), SlopeScaledDepthBias(0.0f),
		DepthClipEnable(true), ScissorEnable(false), MultisampleEnable(false)
{

}

class _ApiExport RasterizerState
{
public:
	RasterizerState(const RasterizerStateDesc& desc)
		: mDesc(desc)
	{

	}

	const RasterizerStateDesc& GetDesc() const				{ return mDesc; }

protected:
	RasterizerStateDesc mDesc;
};

} // Namespace Render
} // Namespace RcEngine

#endif // RasterizerState_h__
