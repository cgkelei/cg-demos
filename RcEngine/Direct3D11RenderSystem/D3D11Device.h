#ifndef D3D11RenderDevice_h__
#define D3D11RenderDevice_h__

#include "D3D11Prerequisites.h"
#include <Graphics/RHDevice.h>

namespace RcEngine {

class D3D11Device;
extern D3D11Device* gD3D11Device;

class _D3D11Export D3D11Device : public RHDevice
{
public:
	D3D11Device(const RenderSettings& settings);
	~D3D11Device();

	inline ID3D11Device* GetDeviceD3D11() const { return mDevice; }

	void ToggleFullscreen(bool fs);
	void AdjustProjectionMatrix(float4x4& pOut);
	void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<RHSamplerState>& state);
	void SetBlendState(const shared_ptr<RHBlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask);
	void SetRasterizerState(const shared_ptr<RHRasterizerState>& state);
	void SetDepthStencilState(const shared_ptr<RHDepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0);

protected:
	void SetViewport(const RHViewport& vp);
	//void DoRender( EffectTechnique& tech, RenderOperation& op );


private:
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Device* mDevice;
	
	typedef std::pair<ShaderType, uint32_t> SamplerSlot;
	std::map<SamplerSlot, shared_ptr<RHSamplerState>> mCurrentSamplers;
};

}



#endif // D3D11RenderDevice_h__
