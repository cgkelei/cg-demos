#ifndef SamplerState_h__
#define SamplerState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {
namespace Render {

class _ApiExport SamplerStateDesc
{
public:
	SamplerStateDesc();

	TextureFilter              Filter;
	TextureAddressMode		   AddressU;
	TextureAddressMode		   AddressV;
	TextureAddressMode		   AddressW;
	float                      MipMapLODBias;
	uint8_t                    MaxAnisotropy;
	CompareFunction            ComparisonFunc;
	float                      MinLOD;
	float                      MaxLOD;

	Math::ColorRGBA			   BorderColor;	
};

inline SamplerStateDesc::SamplerStateDesc()
	: AddressU(TAM_Wrap), AddressV(TAM_Wrap), AddressW(TAM_Wrap), 
		Filter(TF_Min_Mag_Mip_Point), BorderColor(0, 0, 0, 0),
		MipMapLODBias(0),MinLOD(FLT_MIN), MaxLOD( FLT_MAX),
		MaxAnisotropy(16), ComparisonFunc(CF_AlwaysFail)

{
			
}


class _ApiExport SamplerState
{
public:
	SamplerState(const SamplerStateDesc& desc) 
		: mDesc(desc)
	{

	}

	const SamplerStateDesc& GetDesc( ) const				{ return mDesc; }

protected:
	SamplerStateDesc mDesc;
};

} // Namespace Render
} // Namespace RcEngine

#endif // SamplerState_h__
