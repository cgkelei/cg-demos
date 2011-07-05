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
#include <DXGI.h>

#include "../3rdParty/FastDelegate/FastDelegate.h"
#include "../3rdParty/FastDelegate/FastDelegateBind.h"


namespace GlueRH
{
	typedef __int8 int8;
	typedef unsigned __int8 uint8;
	typedef __int16 int16;
	typedef unsigned __int16 uint16;
	typedef __int32 int32;
	typedef unsigned __int32 uint32;
	typedef __int64 int64;
	typedef unsigned __int64 uint64;

	typedef fastdelegate::FastDelegate0<void> EventHandler;
	typedef fastdelegate::FastDelegate1<bool*, void> CancellableEventHandler;

	typedef fastdelegate::FastDelegate1<bool, void> ActiveEvent;
	typedef fastdelegate::FastDelegate0<void> PaintEvent;
	typedef fastdelegate::FastDelegate0<void> EnterSizeMoveEvent;
	typedef fastdelegate::FastDelegate0<void> ExitSizeMoveEvent;
	typedef fastdelegate::FastDelegate1<bool, void> SizeEvent;
	typedef fastdelegate::FastDelegate0<void> SetCursorEvent;
	typedef fastdelegate::FastDelegate0<void> CloseEvent;

	typedef DXGI_FORMAT Format;

	class Window;
	typedef std::shared_ptr<Window> WindowPtr;

	class Application;
	typedef std::shared_ptr<Application> ApplicationPtr;

	class RenderDevice;
	typedef std::shared_ptr<RenderDevice> RenderDevicePtr;

	class GameClock;
	typedef std::shared_ptr<GameClock> GameClockPtr;

	class GameTimer;
	typedef std::shared_ptr<GameTimer> GameTimerPtr;

}



#endif