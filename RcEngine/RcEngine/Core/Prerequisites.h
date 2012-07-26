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
#include <unordered_map>
#include <list>
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <memory>
#include <limits>
#include <cstdint>
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
#include <ois/includes/OISMouse.h>
#include <ois/includes/OISKeyboard.h>
#include <ois/includes/OISInputManager.h>

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


//////////////////////////////////////////////////////////////////////////
typedef std::string String;

using std::shared_ptr;
using std::weak_ptr;
using std::vector;
using std::unordered_map;



#define Safe_Delete(p) if(p) { delete p; p = NULL;}

typedef fastdelegate::FastDelegate0<> EventHandler;
typedef fastdelegate::FastDelegate1<bool*, void> CancellableEventHandler;


namespace RcEngine {

	class Application;
	class Window;
	class Stream;

	namespace Core
	{
		class Timer;
		class Exception;
		class IModule;
		class XMLDoc;
		class XMLNode;
		class XMLAttribute;
		typedef shared_ptr<XMLNode> XMLNodePtr;
		typedef shared_ptr<XMLAttribute> XMLAttributePtr;
	}

	namespace Input
	{
		class InputDevice;
	}

	namespace Render
	{
		struct RenderSettings;
		class RenderDevice;
		class RenderFactory;
		class RenderView;
		class FrameBuffer;
		class Texture;
		class GraphicsBuffer;
		class VertexDeclaration;
		class Camera;
		class Renderable;
		class Mesh;
		class MeshPart;
		class Model;
		class Material;
		class Effect;
		class EffectParameter;
		class EffectTechnique;
		class EffectPass;
		class EffectAnnotation;
		class RenderJob;
		class RenderJobCollection;
		class SimpleBox;
	

	}

} // Namespace RcEngine

#endif // Prerequisites_h__
