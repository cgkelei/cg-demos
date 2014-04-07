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

	virtual void Create() = 0;
	virtual void Release() = 0;
	virtual void ToggleFullscreen(bool fs) = 0;
	virtual bool Fullscreen() const = 0;
	virtual void CreateRenderWindow(const RenderSettings& settings) = 0;	
	virtual void AdjustProjectionMatrix(float4x4& pOut) = 0;

protected:
	virtual void DoBindFrameBuffer(const shared_ptr<FrameBuffer>& fb) = 0;
	virtual void DoRender( EffectTechnique& tech, RenderOperation& op ) = 0;


};

}



#endif // D3D11RenderDevice_h__
