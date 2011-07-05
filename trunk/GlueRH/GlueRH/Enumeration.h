#ifndef _Enumeration__H
#define _Enumeration__H

#include "Common.h"
#include "DeviceSettings.h"

namespace GlueRH
{

	/** In Direct3D 10 an adapter refers to the physical display hardware in the machine.
	 *  An output refers to an output device connected to that physical display hardware. 
	 *  The output device is most often a monitor. Many display adapters will have more than one output attached. 
	 *  For example, if your video card has two DVI connectors on the back, Direct3D 10 will most likely enumerate
	 *  this as on adapter with two outputs attached.
	 */

	struct DeviceInfo;
	struct OutputInfo;
	struct SettingsCombo;

	
	struct AdapterInfo
	{
	private:
		AdapterInfo( const AdapterInfo& );

	public:
		int AdapterOrdinal;
		DXGI_ADAPTER_DESC  AdapterDescription;
		std::string Description;
		std::vector<DeviceInfo> Devices;
		std::vector<OutputInfo> Outputs;
		std::vector<SettingsCombo> SettingsCombos;
		IDXGIAdapter* Adapter;
	};

	struct OutputInfo
	{
	private:
		OutputInfo( const OutputInfo& );
		OutputInfo& operator= ( const OutputInfo& );
	
	public:
		int OutputOrdinal;
		DXGI_OUTPUT_DESC OutputDescription;
		std::vector<DXGI_MODE_DESC> DisplayModes;

		IDXGIOutput* Output;
	};

	struct DeviceInfo
	{
		int32 AdapterOrdinal;
		D3D10_DRIVER_TYPE DriverType ;
	};


	struct SettingsCombo
	{
		int32 AdapterOrdinal;

		D3D10_DRIVER_TYPE DriverType;

		Format BackBufferFormat;

		bool IsWindowed;

		int32 OutputOrdinal;

		std::vector<int32> MultisampleCounts;

		std::vector<int32> MultisampleQualities;

		OutputInfo OutputInfo;
	};

	class Enumeration
	{
	public:
		Enumeration(void);
		~Enumeration(void);

	static DeviceSettings MinimumSettings;

	};

}


#endif

