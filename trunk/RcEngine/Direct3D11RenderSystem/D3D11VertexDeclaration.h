#ifndef D3D11VertexDeclaration_h__
#define D3D11VertexDeclaration_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/VertexDeclaration.h>


namespace RcEngine {

class _D3D11Export D3D11VertexDeclaration : public VertexDeclaration
{
public:
	D3D11VertexDeclaration(void);
	~D3D11VertexDeclaration(void);

	inline ID3D11InputLayout* GetInputLayout() const { return mInputLayout; }

private:
	ID3D11InputLayout* mInputLayout;
};


}




#endif // D3D11VertexDeclaration_h__