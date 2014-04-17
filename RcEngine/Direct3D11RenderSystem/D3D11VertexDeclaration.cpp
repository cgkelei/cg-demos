#include "D3D11VertexDeclaration.h"
#include "D3D11Device.h"
#include "D3D11GraphicCommon.h"

#define MAX_ATTRIBUTES 8

namespace RcEngine {

D3D11VertexDeclaration::D3D11VertexDeclaration( const RHVertexElement* element, uint32_t count )
	: RHVertexDeclaration(element, count),
	  InputLayoutD3D11(nullptr)
{
}

D3D11VertexDeclaration::~D3D11VertexDeclaration(void)
{
	SAFE_RELEASE(InputLayoutD3D11);
}

void D3D11VertexDeclaration::CreateInputLayoutD3D11( std::vector<uint8_t>& code )
{
	static D3D11_INPUT_ELEMENT_DESC layoutD3D11[MAX_ATTRIBUTES]; 

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;

	assert(mVertexElemets.size() <= 8);
	for (size_t i = 0; i < mVertexElemets.size(); ++i)
	{
		const RHVertexElement& element = mVertexElemets[i];
		//layoutD3D11[i].SemanticName = ;
		layoutD3D11[i].SemanticIndex = element.UsageIndex;
		layoutD3D11[i].Format = D3D11Mapping::Mapping(element.Type);
		layoutD3D11[i].InputSlot = element.InputSlot;
		layoutD3D11[i].AlignedByteOffset = element.Offset;
		layoutD3D11[i].InputSlotClass = element.InstanceStepRate > 0 ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		layoutD3D11[i].InstanceDataStepRate = element.InstanceStepRate;
	}

	HRESULT hr;
	hr = deviceD3D11->CreateInputLayout( layoutD3D11, mVertexElemets.size(), &code[0], code.size(), &InputLayoutD3D11 );

	if (FAILED(hr))
	{

	}
}

}

