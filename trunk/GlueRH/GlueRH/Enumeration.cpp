#include "Enumeration.h"

namespace GlueRH
{

	Enumeration::Enumeration(void)
	{
	}


	Enumeration::~Enumeration(void)
	{
	}

	void GlueRH::Enumeration::Enumerate( IDXGIFactory* pFactory )
	{
		UINT i = 0; 
		IDXGIAdapter * pAdapter; 
		std::vector <IDXGIAdapter*> vAdapters; 
		while( pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND ) 
		{ 
			vAdapters.push_back(pAdapter); 
			++i; 
		} 



	}

}