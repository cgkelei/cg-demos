#ifndef _Common__H
#define _Common__H

#include <windows.h>
#include <string>
#include <memory>
#include <exception>
#include <list>
#include <map>
#include <vector>
#include <cassert>
#include <D3D10.h>
#include <D3DX10.h>


#include "../3rdParty/FastDelegate/FastDelegate.h"
#include "../3rdParty/FastDelegate/FastDelegateBind.h"


namespace GlueRH
{
	typedef char int8;
	typedef unsigned char uint8;
	typedef short int16;
	typedef unsigned int short uint16;
	typedef int int32;
	typedef unsigned int uint32;

	typedef fastdelegate::FastDelegate1<bool, void> ActiveEvent;
	typedef fastdelegate::FastDelegate0<void> PaintEvent;
	typedef fastdelegate::FastDelegate0<void> EnterSizeMoveEvent;
	typedef fastdelegate::FastDelegate0<void> ExitSizeMoveEvent;
	typedef fastdelegate::FastDelegate1<bool, void> SizeEvent;	typedef fastdelegate::FastDelegate0<void> SetCursorEvent;
	typedef fastdelegate::FastDelegate0<void> CloseEvent;

	typedef DXGI_FORMAT PiexelFormat;
	typedef DXGI_FORMAT DepthFormat;


	class RenderDevice;
	typedef std::shared_ptr<RenderDevice> RenderDevicePtr;
}



#endif