#include "Enumeration.h"
#include <string>

namespace GlueRH
{

	bool Enumeration::HasEnumerated = false;
	bool Enumeration::HasMinimumSettings = false;
	static std::vector<AdapterInfoPtr> Adapters;
	static DeviceSettings MinimumSettings;


	Enumeration::Enumeration(void)
	{
	}


	Enumeration::~Enumeration(void)
	{
	}

	void Enumeration::EnumerateSettingsCombos(  AdapterInfoPtr& adapterInfo )
	{

		DXGI_FORMAT backBufferFormats[] = { DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R10G10B10A2_UNORM };

		for( auto outputInfo = adapterInfo->Outputs.begin(); outputInfo != adapterInfo->Outputs.end(); ++outputInfo)
		{
			for(auto deviceInfo = adapterInfo->Devices.begin(); deviceInfo != adapterInfo->Devices.end(); ++deviceInfo)
			{
				for(size_t i = 0; i < sizeof(backBufferFormats) / sizeof(DXGI_FORMAT); i++)
				{
					DXGI_FORMAT backBufferFormat = backBufferFormats[i];
					if(HasMinimumSettings && MinimumSettings.BackBufferFormat != DXGI_FORMAT_UNKNOWN &&
						MinimumSettings.BackBufferFormat != backBufferFormat)
					{
						continue;
					}

					for (int windowed = 0; windowed < 2; windowed++)
					{
						if (windowed == 0 && (*outputInfo)->DisplayModes.size() == 0)
							continue;

						bool found = false;
						for( auto mode =  (*outputInfo)->DisplayModes.begin(); mode != (*outputInfo)->DisplayModes.end(); ++mode )
						{
							if ( mode->Format == backBufferFormat)
							{
								found = true;
								break;
							}
						}
						if (!found)
							continue;

						SettingsComboPtr combo = std::make_shared<SettingsCombo>();
						combo->AdapterOrdinal = deviceInfo->AdapterOrdinal;
						combo->DriverType = deviceInfo->DriverType;
						combo->BackBufferFormat = backBufferFormat;
						combo->IsWindowed = (windowed != 0);
						combo->OutputOrdinal = (*outputInfo)->OutputOrdinal;
						combo->OutputInfo = (*outputInfo);

						BuildMultisampleList(backBufferFormat, combo);

						/*if (HasMinimumSettings && !combo.MultisampleCounts.Contains(ConversionMethods.ToDirect3D10(MinimumSettings.MultisampleType, 0)))
						{
							continue;
						}*/

						adapterInfo->SettingsCombos.push_back(combo);
					}
				}
			}
		}
	}

	void Enumeration::EnumerateDevices(  AdapterInfoPtr& adapterInfo )
	{
		D3D10_DRIVER_TYPE allowedTypes[] = { D3D10_DRIVER_TYPE_HARDWARE, D3D10_DRIVER_TYPE_REFERENCE };

		for ( size_t i = 0; i < sizeof(allowedTypes) / sizeof(D3D10_DRIVER_TYPE); i++)
		{
			if ( HasMinimumSettings && MinimumSettings.DriverType != allowedTypes[i] )
			{
				continue;
			}

			//if not has MinimumSettings, build one
			DeviceInfo deviceInfo;
			deviceInfo.AdapterOrdinal = adapterInfo->AdapterOrdinal;
			deviceInfo.DriverType = allowedTypes[i] ;

			IDXGIAdapter* pAdapter = ( allowedTypes[i] == D3D10_DRIVER_TYPE_HARDWARE ) ? adapterInfo->Adapter : NULL;
			
			ID3D10Device* pDevice = NULL;
			HR( D3D10CreateDevice(pAdapter, allowedTypes[i], NULL, 0, D3D10_SDK_VERSION, &pDevice) );
	
			if (allowedTypes[i] != D3D10_DRIVER_TYPE_HARDWARE)
			{
				IDXGIDevice * pDXGIDevice;
				HR( pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice) );

				if ( adapterInfo->Adapter != NULL )
				{
					SafeRelease(&adapterInfo->Adapter);
				}

				pDXGIDevice->GetAdapter( &adapterInfo->Adapter );
				
				SafeRelease( &pDXGIDevice );
			}

			SafeRelease( &pDevice );
			adapterInfo->Devices.push_back(deviceInfo);
			
		}
	}

	void Enumeration::EnumerateDisplayModes(  OutputInfoPtr& outputInfo )
	{
		DXGI_FORMAT allowedFormats[] = { DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
												DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R10G10B10A2_UNORM };

		for(size_t i = 0; i < sizeof(allowedFormats) / sizeof(DXGI_FORMAT); i++)
		{

			UINT num = 0;
			outputInfo->Output->GetDisplayModeList( allowedFormats[i], DXGI_ENUM_MODES_SCALING, &num, 0);

			DXGI_MODE_DESC* pDescs = new DXGI_MODE_DESC[num];
			outputInfo->Output->GetDisplayModeList( allowedFormats[i], DXGI_ENUM_MODES_SCALING, &num, pDescs);

			for(UINT j = 0; j < num; j++)
			{
				if (HasMinimumSettings)
				{
					if ( pDescs[j].Width < static_cast<UINT>(MinimumSettings.BackBufferWidth) ||
						pDescs[j].Height < static_cast<UINT>(MinimumSettings.BackBufferHeight) ||
						(int)ToFloat(pDescs[j].RefreshRate) < MinimumSettings.RefreshRate)
					{
						continue;
					}

					outputInfo->DisplayModes.push_back(pDescs[j]);
				}
			}

			delete[] pDescs;
		}
	}

	void Enumeration::EnumerateOutputs(  AdapterInfoPtr& adapterInfo )
	{
		assert(adapterInfo->Adapter != NULL);

		UINT i = 0;
		IDXGIOutput * pOutput;
		while(adapterInfo->Adapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			OutputInfoPtr outputInfo = std::make_shared<OutputInfo>();
			HR( pOutput->GetDesc( &outputInfo->OutputDescription ) );
			outputInfo->OutputOrdinal = i;
			outputInfo->Output = pOutput;

			EnumerateDisplayModes(outputInfo);
			if (outputInfo->DisplayModes.size() == 0)
				continue;

			adapterInfo->Outputs.push_back(outputInfo);
		}
	}

	GlueRH::OutputInfoPtr Enumeration::GetOutputInfo( int32 adapterOrdinal, int32 outputOrdinal )
	{
		AdapterInfoPtr adapter;
		for(auto iter = Adapters.begin(); iter != Adapters.end(); ++iter)
        { 
			if (adapterOrdinal == (*iter)->AdapterOrdinal)
            {
				adapter = (*iter);
				break;
            }
        }
		if(adapter)
		{
			if( (int32)adapter->Outputs.size() > outputOrdinal )
				return adapter->Outputs[outputOrdinal];
		}
        throw std::exception("No Such Output");
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

		/*bool unique = true;
		for( auto iter1 = Adapters.begin(); iter1 != Adapters.end(); ++iter1 )
		{
		for( auto iter2 = Adapters.begin(); iter2 != Adapters.end(); ++iter2 )
		{
		if ( (*iter1)->Adapter == (*iter2)->Adapter )
		continue;
		if ( wcscmp( (*iter1)->AdapterDescription.Description , (*iter2)->AdapterDescription.Description) ) ==  0 )
		{
		unique = false;
		break;
		}
		}

		if (!unique)
		break;
		}*/

		for( auto iter = Adapters.begin(); iter != Adapters.end(); ++iter )
		{
			(*iter)->Description = (*iter)->AdapterDescription.Description;
		}
	}

	float Enumeration::ToFloat( DXGI_RATIONAL rational )
	{
		float denom = 1;
		if (rational.Denominator != 0)
			denom = static_cast<float>( rational.Denominator );
		return rational.Numerator / denom;
	}

}