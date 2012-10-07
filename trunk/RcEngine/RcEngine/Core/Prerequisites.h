#ifndef Prerequisites_h__
#define Prerequisites_h__

////////////////////////////////////////////////////////////////////////////////
// Compile Define
////////////////////////////////////////////////////////////////////////////////
#include "CompileConfig.h"

////////////////////////////////////////////////////////////////////////////////
// API.
////////////////////////////////////////////////////////////////////////////////
//#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
//#endif

////////////////////////////////////////////////////////////////////////////////
// StableSTLHeader
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <functional>
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
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <FastDelegate/FastDelegate.h>
#include <FastDelegate/FastDelegateBind.h>
#include <TinyThread/source/tinythread.h>
#include <TinyThread/source/fast_mutex.h>

typedef fastdelegate::FastDelegate0<> EventHandler;
typedef fastdelegate::FastDelegate1<bool*, void> CancellableEventHandler;
typedef fastdelegate::FastDelegate3<uint32_t, bool, float> InputActionHandler;
typedef fastdelegate::FastDelegate3<uint32_t, bool, float> InputStateHandler;
typedef fastdelegate::FastDelegate3<uint32_t, int32_t, float> InputRangeHandler;

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
using std::unordered_set;
using namespace tthread;


#define Safe_Delete(p) if(p) { delete p; p = NULL;}

// for normal pointer 
template<class Type, class SourceType>
Type static_cast_checked(SourceType* item)
{
	assert(!item || dynamic_cast<Type>(item));
	return static_cast<Type>(item);
}

// for shared_ptr
template <class Ty, class Other>
shared_ptr<Ty> static_pointer_cast_checked(const shared_ptr<Other>& sp)
{
	assert(!sp || std::dynamic_pointer_cast<Ty>(sp));
	return std::static_pointer_cast<Ty>(sp);
}


namespace RcEngine {

class Application;
class Window;
class Stream;
class Timer;
class Exception;
class IModule;
class XMLDoc;
class XMLNode;
class XMLAttribute;
typedef shared_ptr<XMLNode> XMLNodePtr;
typedef shared_ptr<XMLAttribute> XMLAttributePtr;

class Resource;
class ResourceManager;
class InputSystem;
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
class RenderOperation;
class SimpleBox;
class DepthStencilState;
class RasterizerState;
class BlendState;
class SamplerState;
class SceneManager;
class SceneNode;
class Entity;
class AnimationPlayer;
class AnimationState;
class AnimationClip;
class AnimationController;


} // Namespace RcEngine

#endif // Prerequisites_h__
