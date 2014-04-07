#ifndef D3D11GraphicCommon_h__
#define D3D11GraphicCommon_h__

#include "D3D11Prerequisites.h"
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>

namespace RcEngine {

class _D3D11Export D3D11Mapping
{
public:

	static void MapUsage(uint32_t accessHint, D3D11_USAGE& usage, UINT& CPUAccessFlags); 
	static DXGI_FORMAT Mapping(PixelFormat inPixelFormat);

};

}




#endif // D3D11GraphicCommon_h__
