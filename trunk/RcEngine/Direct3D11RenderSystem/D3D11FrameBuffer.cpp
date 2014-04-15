#include "D3D11FrameBuffer.h"
#include "D3D11Device.h"
#include "D3D11View.h"

namespace RcEngine {

D3D11FrameBuffer::D3D11FrameBuffer( uint32_t width, uint32_t height, bool offscreen /*= true*/ )
	: RHFrameBuffer(width, height, offscreen)
{

}

void D3D11FrameBuffer::SwapBuffers()
{

}

void D3D11FrameBuffer::OnBind()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->GetDeviceContextD3D11();

	vector<ID3D11RenderTargetView*> rtvD3D11;
	for (const auto& colorView : mColorViews)
	{
		if (colorView)
		{
			rtvD3D11.push_back((static_cast_checked<D3D11TargetView*>(colorView.get())->RenderTargetViewD3D11));
		}
	}

	ID3D11DepthStencilView* dsvD3D11 = nullptr;
	if (mDepthStencilView)
		dsvD3D11 = (static_cast_checked<D3D11DepthStencilView*>(mDepthStencilView.get())->DepthStencilViewD3D11);

	if (mUnorderedAccessViews.size())
	{		
		vector<ID3D11UnorderedAccessView*> uavD3D11;
		for (const auto& uavView : mUnorderedAccessViews)
		{
			if (uavView->GetViewDimension() == UAV_Texture)
				uavD3D11.push_back(static_cast_checked<D3D11TextureUAV*>(uavView.get())->UnorderedAccessViewD3D11);
			else if (uavView->GetViewDimension() == UAV_Buffer)
				uavD3D11.push_back(static_cast_checked<D3D11BufferUAV*>(uavView.get())->UnorderedAccessViewD3D11);

		}
		
		deviceContextD3D11->OMSetRenderTargetsAndUnorderedAccessViews(rtvD3D11.size(), &rtvD3D11[0], dsvD3D11,
			0, uavD3D11.size(), &uavD3D11[0], nullptr);
	}
	else
	{
		deviceContextD3D11->OMSetRenderTargets(rtvD3D11.size(), &rtvD3D11[0], dsvD3D11);
	}
}

void D3D11FrameBuffer::OnUnbind()
{

}





}


