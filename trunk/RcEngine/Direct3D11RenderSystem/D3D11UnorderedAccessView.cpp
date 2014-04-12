#include "D3D11UnorderedAccessView.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Texture.h"
#include "D3D11Buffer.h"

namespace RcEngine {

ID3D11Device* g_pD3D11Device;

D3D11BufferUAV::D3D11BufferUAV( const shared_ptr<RHBuffer>& buffer )
	: RHBufferUAV(buffer),
	  UnorderedAccessViewD3D11(nullptr)
{

}

D3D11BufferUAV::~D3D11BufferUAV()
{
	SAFE_RELEASE(UnorderedAccessViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureBufferUAV::D3D11TextureBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: D3D11BufferUAV(buffer)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = D3D11Mapping::Mapping(format);
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	
	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = g_pD3D11Device->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11StructuredBufferUAV::D3D11StructuredBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: D3D11BufferUAV(buffer)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = g_pD3D11Device->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureUAV::D3D11TextureUAV( const shared_ptr<RHTexture>& texture )
	: RHTextureUAV(texture),
	  UnorderedAccessViewD3D11(nullptr)
{

}

D3D11TextureUAV::~D3D11TextureUAV()
{
	SAFE_RELEASE(UnorderedAccessViewD3D11);
}

D3D11Texture2DUAV::D3D11Texture2DUAV( const shared_ptr<RHTexture>& texture, uint32_t level )
	: D3D11TextureUAV(texture)
{
	D3D11Texture2D* textureD3D11 = (static_cast<D3D11Texture2D*>(texture.get()));

	uint32_t createFlags = textureD3D11->GetCreateFlags();

	assert(textureD3D11->GetTextureArraySize() <= 1);
	assert(createFlags & TexCreate_UAV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = level;

	HRESULT hr = g_pD3D11Device->CreateUnorderedAccessView(textureD3D11->TextureD3D11, &desc, &UnorderedAccessViewD3D11);
}


}


