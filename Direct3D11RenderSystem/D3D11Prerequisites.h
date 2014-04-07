#ifndef D3D11Prerequisites_h__
#define D3D11Prerequisites_h__

#include <d3d11.h>

//////////////////////////////////////////////////////////////////////////
#include <Core/Prerequisites.h>


//////////////////////////////////////////////////////////////////////////
#ifndef BUILD_STATIC
#	ifdef D3D11ENGINEDLL_EXPORTS
#		define _D3D11Export __declspec(dllexport)
#	else
#    	define _D3D11Export __declspec(dllimport) 
#	endif
#else
#	define _D3D11Export
#endif	


#endif // D3D11Prerequisites_h__
