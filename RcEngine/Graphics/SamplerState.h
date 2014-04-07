#ifndef SamplerState_h__
#define SamplerState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

#pragma pack(push, 1)

struct _ApiExport SamplerStateDesc
{
public:
	SamplerStateDesc();

	ShaderType BindStage;

	TextureFilter              Filter;
	TextureAddressMode		   AddressU;
	TextureAddressMode		   AddressV;
	TextureAddressMode		   AddressW;
	float                      MipLODBias;
	uint8_t                    MaxAnisotropy;
	CompareFunction            ComparisonFunc;
	float                      MinLOD;
	float                      MaxLOD;

	ColorRGBA			       BorderColor;	

	friend bool operator< (const SamplerStateDesc& lhs, const SamplerStateDesc& rhs)
	{
		return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
	}
};

#pragma pack(pop)

class _ApiExport SamplerState
{
public:
	SamplerState(const SamplerStateDesc& desc) 
		: mDesc(desc) { }

	virtual ~SamplerState() { }

	const SamplerStateDesc& GetDesc( ) const				{ return mDesc; }

protected:
	SamplerStateDesc mDesc;
};

} // Namespace RcEngine

#endif // SamplerState_h__
