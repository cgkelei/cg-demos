#ifndef D3D11RenderDevice_h__
#define D3D11RenderDevice_h__

#include "D3D11Prerequisites.h"
#include <Graphics/RenderDevice.h>

namespace RcEngine {

class _D3D11Export D3D11RenderDevice : public RenderDevice
{
public:
	D3D11RenderDevice();
	~D3D11RenderDevice();

	void Create();
	void Release();
	void ToggleFullscreen(bool fs);
	bool Fullscreen() const;
	void CreateRenderWindow(const RenderSettings& settings);	
	void AdjustProjectionMatrix(float4x4& pOut);

protected:
	void DoBindFrameBuffer(const shared_ptr<FrameBuffer>& fb);
	void DoRender( EffectTechnique& tech, RenderOperation& op );


private:


};

}



#endif // D3D11RenderDevice_h__
