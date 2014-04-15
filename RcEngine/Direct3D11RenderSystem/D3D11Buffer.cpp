#include "D3D11Buffer.h"
#include "D3D11GraphicCommon.h"

namespace RcEngine {

ID3D11Device* g_pDevice;

D3D11Buffer::D3D11Buffer( uint32_t bufferSize, uint32_t accessHint, uint32_t flags, D3D11_BIND_FLAG target, uint32_t structSize, ElementInitData* initData )
	: RHBuffer(bufferSize, accessHint, flags),
	  BufferD3D11(nullptr)
{
	D3D11_BUFFER_DESC desc;
	
	desc.ByteWidth = bufferSize;
	desc.StructureByteStride = 0;
	desc.MiscFlags = 0;
	desc.BindFlags = target;
	
	D3D11Mapping::Mapping(accessHint, desc.Usage, desc.CPUAccessFlags);

	if (flags & BufferCreate_StreamOutput)
	{
		desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	}
	else if (flags & BufferCreate_Structured)
	{
		desc.StructureByteStride = structSize;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	if (accessHint & (EAH_GPU_Write | EAH_GPU_Read))
	{
		// GPU read/write
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		assert(desc.Usage == D3D11_USAGE_DEFAULT);
	}
	else if (accessHint & EAH_GPU_Read)
	{
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}
	else
	{
		// Error
	}

	HRESULT hr;
	if (initData)
	{
		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = initData->pData;
		subData.SysMemPitch = initData->rowPitch;

		hr = g_pDevice->CreateBuffer( &desc, &subData, &BufferD3D11 );
	}
	else 
	{
		hr = g_pDevice->CreateBuffer( &desc, NULL, &BufferD3D11 );
	}
	
	assert(SUCCEEDED(hr));
}


D3D11Buffer::~D3D11Buffer(void)
{
}

void* D3D11Buffer::Map( uint32_t offset, uint32_t length, ResourceMapAccess options )
{
	return NULL;
}

void D3D11Buffer::UnMap()
{

}

void D3D11Buffer::ResizeBuffer( uint32_t size )
{

}

}


