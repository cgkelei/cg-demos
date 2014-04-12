#ifndef RHState_h__
#define RHState_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

#define	DEFAULT_STENCIL_READ_MASK	( 0xff )
#define	DEFAULT_STENCIL_WRITE_MASK	( 0xff )

#pragma pack(push, 1)

struct _ApiExport RHDepthStencilStateDesc
{
	RHDepthStencilStateDesc();

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

	friend bool operator< (const RHDepthStencilStateDesc& lhs, const RHDepthStencilStateDesc& rhs);
};

#pragma pack(pop)

class _ApiExport RHDepthStencilState
{
public:
	RHDepthStencilState(const RHDepthStencilStateDesc& desc) 
		: mDesc( desc ) {}
	virtual ~RHDepthStencilState() {}

	inline const RHDepthStencilStateDesc& GetDesc() const	{ return mDesc; } 

protected:
	RHDepthStencilStateDesc mDesc;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

struct _ApiExport RHBlendStateDesc
{
public:
	struct RenderTargetBlendDesc
	{
		RenderTargetBlendDesc();

		bool				BlendEnable;
		AlphaBlendFactor    SrcBlend;
		AlphaBlendFactor    DestBlend;
		BlendOperation		BlendOp;
		AlphaBlendFactor    SrcBlendAlpha;
		AlphaBlendFactor    DestBlendAlpha;
		BlendOperation		BlendOpAlpha;
		uint8_t				ColorWriteMask;
	};

public:
	RHBlendStateDesc();

	bool AlphaToCoverageEnable;
	bool IndependentBlendEnable;
	RenderTargetBlendDesc RenderTarget[8];

	friend bool operator< (const RHBlendStateDesc& lhs, const RHBlendStateDesc& rhs);
};

#pragma pack(pop)

class _ApiExport RHBlendState
{
public:
	RHBlendState(const RHBlendStateDesc& desc) 
		: mDesc(desc) { }
	virtual ~RHBlendState() {}

	inline const RHBlendStateDesc& GetDesc() const { return mDesc; }

protected:
	RHBlendStateDesc mDesc;
};

//////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)

struct _ApiExport RHRasterizerStateDesc
{
public:
	RHRasterizerStateDesc();

	FillMode		PolygonFillMode;
	CullMode		PolygonCullMode;
	bool            FrontCounterClockwise;
	float           DepthBias;
	float           SlopeScaledDepthBias;
	bool            DepthClipEnable;
	bool            ScissorEnable;
	bool            MultisampleEnable;

	friend bool operator< (const RHRasterizerStateDesc& lhs, const RHRasterizerStateDesc& rhs);
};

#pragma pack(pop)


class _ApiExport RHRasterizerState
{
public:
	RHRasterizerState(const RHRasterizerStateDesc& desc)
		: mDesc(desc) { }
	virtual ~RHRasterizerState() {}

	inline const RHRasterizerStateDesc& GetDesc() const { return mDesc; }

protected:
	RHRasterizerStateDesc mDesc;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

struct _ApiExport RHSamplerStateDesc
{
public:
	RHSamplerStateDesc();

	ShaderType				   BindStage;
	bool					   CompareSampler;

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

	friend bool operator< (const RHSamplerStateDesc& lhs, const RHSamplerStateDesc& rhs);
};

#pragma pack(pop)

class _ApiExport RHSamplerState
{
public:
	RHSamplerState(const RHSamplerStateDesc& desc) 
		: mDesc(desc) { }
	virtual ~RHSamplerState() { }

	inline const RHSamplerStateDesc& GetDesc( ) const { return mDesc; }

protected:
	RHSamplerStateDesc mDesc;
};

}

#endif // RHState_h__
