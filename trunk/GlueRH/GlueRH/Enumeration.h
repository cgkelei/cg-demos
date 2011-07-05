#ifndef _Enumeration__H
#define _Enumeration__H

#include <Common.h>

namespace GlueRH
{

	struct AdapterInfo
	{
		int AdapterOrdinal;

		DXGI_ADAPTER_DESC  AdapterDescription;

		IDXGIAdapter* Adapter;


	};

	struct OutputInfo
	{
		int OutputOrdinal;
		DXGI_OUTPUT_DESC OutputDescription;
		IDXGIOutput* Output;
	};


	class Enumeration
	{
	public:
		Enumeration(void);
		~Enumeration(void);
	};

}


#endif

