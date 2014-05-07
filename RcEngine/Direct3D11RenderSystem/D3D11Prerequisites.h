#ifndef D3D11Prerequisites_h__
#define D3D11Prerequisites_h__

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <comdef.h>

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

//
////////////////////////////////////////////////////////////////////////////
//#if defined(DEBUG) || defined(_DEBUG)
//#ifndef V
//#define V(x)           { HRESULT hr = (x); if( FAILED(hr) ) { DXTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
//#endif
//#ifndef V_RETURN
//#define V_RETURN(x)    { HRESULT hr = (x); if( FAILED(hr) ) { return DXTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
//#endif
//#else
//#ifndef V
//#define V(x)           { hr = (x); }
//#endif
//#ifndef V_RETURN
//#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
//#endif
//#endif


#if defined(DEBUG) || defined(_DEBUG)
	#ifndef D3D11_VERRY
	#define D3D11_VERRY(x)							\
	{												\
		HRESULT hr = (x);							\
		if (FAILED(hr))								\
		{											\
			fprintf(stderr, "File:%s\nLine:%d\nCalling:%s\n\n", __FILE__, __LINE__, __FUNCTION__); \
		}	\
	}
	#endif 
#else
#define V_RETURN(x) {(x);}
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#endif // D3D11Prerequisites_h__
