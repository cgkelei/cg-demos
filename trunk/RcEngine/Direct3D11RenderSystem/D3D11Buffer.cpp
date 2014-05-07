#include "D3D11Buffer.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11Buffer::D3D11Buffer( uint32_t bufferSize, uint32_t accessHint, uint32_t flags, uint32_t structSize, ElementInitData* initData )
	: GraphicsBuffer(bufferSize, accessHint, flags),
	  BufferD3D11(nullptr)
{
	D3D11_BUFFER_DESC desc;
	D3D11Mapping::Mapping(accessHint, desc.Usage, desc.CPUAccessFlags);

	desc.ByteWidth = bufferSize;
	desc.StructureByteStride = structSize;
	desc.MiscFlags = 0;
	desc.BindFlags = 0;

	if (mCreateFlags & BufferCreate_Uniform)
	{
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	}
	else if (mCreateFlags & BufferCreate_Index)
	{
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	}
	else
	{
		if (mCreateFlags & BufferCreate_Vertex)
			desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;

		if (mCreateFlags & BufferCreate_StreamOutput)
			desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;

		if (mCreateFlags & (BufferCreate_Texture | BufferCreate_Structured) )
		{
			desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = structSize;

			if (accessHint & EAH_GPU_Write)
			{
				desc.BindFlags |=  D3D11_BIND_UNORDERED_ACCESS;
				assert(desc.Usage == D3D11_USAGE_DEFAULT);
			}
		}
	}

	HRESULT hr;
	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	if (initData)
	{
		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = initData->pData;
		subData.SysMemPitch = initData->rowPitch;

		hr = deviceD3D11->CreateBuffer( &desc, &subData, &BufferD3D11 );
	}
	else 
	{
		hr = deviceD3D11->CreateBuffer( &desc, NULL, &BufferD3D11 );
	}
	
	assert(SUCCEEDED(hr));
}


D3D11Buffer::~D3D11Buffer(void)
{
	SAFE_RELEASE(BufferD3D11);
}

void* D3D11Buffer::Map( uint32_t offset, uint32_t length, ResourceMapAccess options )
{
	if (length == MAP_ALL_BUFFER)
	{
		assert(offset < mBufferSize);
		length = mBufferSize - offset;
	}
	else
	{
		if (offset + length > mBufferSize)
			ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Out of range!", "D3D11Buffer::Map");
	}

	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	
	D3D11_MAPPED_SUBRESOURCE mappedD3D11;
	HRESULT hr = deviceContextD3D11->Map(BufferD3D11, 0, D3D11Mapping::Mapping(options), 0, &mappedD3D11);

	return (uint8_t*)mappedD3D11.pData + offset;
}

void D3D11Buffer::UnMap()
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	deviceContextD3D11->Unmap(BufferD3D11, 0);
}

void D3D11Buffer::ResizeBuffer( uint32_t size )
{

}

}


