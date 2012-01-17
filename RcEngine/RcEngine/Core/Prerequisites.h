#ifndef Prerequisites_h__
#define Prerequisites_h__

////////////////////////////////////////////////////////////////////////////////
// Compile Define
////////////////////////////////////////////////////////////////////////////////
#include "CompileConfig.h"

////////////////////////////////////////////////////////////////////////////////
// API.
////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// StableSTLHeader
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <map>
#include <list>
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Dependencies
////////////////////////////////////////////////////////////////////////////////
// ANSI standard includes.
#include <memory.h>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cstdarg>
#include <ctime>

// 3rd party includes.
#include <FastDelegate/FastDelegate.h>
#include <FastDelegate/FastDelegateBind.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

//------------------------------------------------------------------------------
// Dll Define
//------------------------------------------------------------------------------
#	ifdef BUILD_STATIC
#		define _ApiExport
#	else 
#		ifdef BUILD_DLL
#			define _ApiExport __declspec( dllexport )
#		else     
#			define _ApiExport __declspec( dllimport )
#		endif
#	endif


//------------------------------------------------------------------------------
//brief Global types for RcEngine, encapsulates main datatypes and guarantees the
//correct size in bytes.
//------------------------------------------------------------------------------
typedef unsigned char		uint8;
typedef signed char			int8;
typedef unsigned short		uint16;
typedef signed short		int16;
typedef unsigned int		uint32;
typedef signed int			int32;
typedef signed long long	int64;
typedef unsigned long long  uint64;

//RcEngine String 
typedef std::string String;


#define Safe_Delete(p) if(p) { delete p; p = NULL;}

typedef fastdelegate::FastDelegate0<> EventHandler;
typedef fastdelegate::FastDelegate1<bool*, void> CancellableEventHandler;


namespace RcEngine {

	class Application;
	class Window;

	namespace Core
	{
		class Timer;
		class Exception;
	}

	namespace RenderSystem
	{
		struct RenderSettings;
		class RenderDevice;
		class RenderFactory;
		class RenderView;
		class RenderWindow;
		class RenderEffect;
		class RenderParameter;
		class RenderPass;
		class RenderTechnique;
		class FrameBuffer;
		class Texture;
		class GraphicBuffer;
		class VertexDeclaration;
		class Camera;

	}

} // Namespace RcEngine

#endif // Prerequisites_h__
