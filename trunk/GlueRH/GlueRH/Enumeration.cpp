#include "Enumeration.h"

namespace GlueRH
{

	Enumeration::Enumeration(void)
	{
	}


	Enumeration::~Enumeration(void)
	{
	}

	void Enumeration::EnumerateSettingsCombos(  AdapterInfoPtr& adapterInfo )
	{

	}

	void Enumeration::EnumerateDevices(  AdapterInfoPtr& adapterInfo )
	{

	}

	void Enumeration::EnumerateDisplayModes(  OutputInfoPtr& outputInfo )
	{

	}

	void Enumeration::EnumerateOutputs(  AdapterInfoPtr& adapterInfo )
	{

	}

	GlueRH::OutputInfoPtr Enumeration::GetOutputInfo( int adapterOrdinal, int outputOrdinal )
	{

	}

	void GlueRH::Enumeration::Enumerate( IDXGIFactory* pFactory )
	{
		HasEnumerated = true;

		UINT i = 0; 
		IDXGIAdapter * pAdapter; 
		while( pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND ) 
		{ 
			AdapterInfoPtr adapter = std::make_shared<AdapterInfo>();
			pAdapter->GetDesc( & adapter->AdapterDescription );
			adapter->AdapterOrdinal = i;
			adapter->Adapter = pAdapter;

			EnumerateDevices(adapter);
			if( adapter->Devices.size() == 0 )
				continue;
			EnumerateOutputs(adapter);
			if( adapter->Outputs.size() == 0 )
				continue;

			EnumerateSettingsCombos(adapter);
			if( adapter->SettingsCombos.size() == 0 )
				continue;

			Adapters.push_back(adapter);
			++i; 
		} 
	}

}