#ifndef _Direct3DUtil__H
#define _Direct3DUtil__H

#include <DxErr.h>

namespace GlueRH
{
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
		#define HR(x)                                                \
		{                                                            \
			HRESULT hr = (x);                                        \
			if(FAILED(hr))                                           \
			{                                                        \
				DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);   \
			}                                                        \
		}
	#endif
#else
	#ifndef HR
		#define HR(x) (x)
	#endif
#endif


	template <class T> 
	void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}

}


#endif