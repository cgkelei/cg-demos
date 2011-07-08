#ifndef _Enumeration__H
#define _Enumeration__H

#include "Common.h"
#include "DeviceSettings.h"
#include "Direct3DUtil.h"


namespace GlueRH
{

	/** In Direct3D 10 an adapter refers to the physical display hardware in the machine.
	 *  An output refers to an output device connected to that physical display hardware. 
	 *  The output device is most often a monitor. Many display adapters will have more than one output attached. 
	 *  For example, if your video card has two DVI connectors on the back, Direct3D 10 will most likely enumerate
	 *  this as on adapter with two outputs attached.
	 */

	struct DeviceInfo
	{
		int32 AdapterOrdinal;
		D3D10_DRIVER_TYPE DriverType ;
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

	public:
		OutputInfo() : Output(0) {}
		~OutputInfo() { SafeRelease(&Output); }
	};
	
	typedef std::shared_ptr<OutputInfo> OutputInfoPtr;


	struct SettingsCombo
	{
		int32 AdapterOrdinal;
		int32 OutputOrdinal;
		D3D10_DRIVER_TYPE DriverType;
		Format BackBufferFormat;
		bool IsWindowed;
		std::vector<int32> MultisampleCounts;
		std::vector<int32> MultisampleQualities;
		OutputInfoPtr OutputInfo;
	};

	typedef std::shared_ptr<SettingsCombo> SettingsComboPtr;
		

	struct AdapterInfo
	{
	private:
		AdapterInfo( const AdapterInfo& );
		AdapterInfo& operator= ( const AdapterInfo& );

	public:
		int AdapterOrdinal;
		DXGI_ADAPTER_DESC  AdapterDescription;
		std::wstring Description;
		std::vector<DeviceInfo> Devices;
		std::vector<OutputInfoPtr> Outputs;
		std::vector<SettingsComboPtr> SettingsCombos;
		IDXGIAdapter* Adapter;

	public:
		AdapterInfo() : Adapter( 0 ) {}
		~AdapterInfo()  { SafeRelease(&Adapter); } 
	};

	
	typedef std::shared_ptr<AdapterInfo> AdapterInfoPtr;

	

	class Enumeration
	{
	private:
		Enumeration(void);
		~Enumeration(void);

	public:

		static OutputInfoPtr GetOutputInfo(int32 adapterOrdinal, int32 outputOrdinal);

		static void EnumerateOutputs( AdapterInfoPtr& adapterInfo);

		static void EnumerateDisplayModes( OutputInfoPtr& outputInfo);

		static void EnumerateDevices( AdapterInfoPtr& adapterInfo);

		static void EnumerateSettingsCombos( AdapterInfoPtr& adapterInfo);

		static void Enumerate( IDXGIFactory* pFactory );

		static void BuildMultisampleList(DXGI_FORMAT backBufferFormat, const SettingsComboPtr& combo);

		static const std::vector<AdapterInfoPtr>&  GetAdapters() ;

	private:

		/** The DXGI_RATIONAL structure operates under the following rules:
		 *  0/0 is legal and will be interpreted as 0/1. 
		 *  0/anything is interpreted as zero. 
		 *  If you are representing a whole number, the denominator should be 1.
		 */
		static float ToFloat(DXGI_RATIONAL rational);


	private:
		static std::vector<AdapterInfoPtr> Adapters;
		static bool HasEnumerated;
		static DeviceSettings MinimumSettings;
		static bool HasMinimumSettings;


	};

}


#endif

