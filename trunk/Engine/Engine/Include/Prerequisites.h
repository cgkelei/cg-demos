#ifndef _Prerequisites__H
#define _Prerequisites__H

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <map>
#include <vector>
#include <list>
#include <stack>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <cmath>


namespace Engine
{

// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define ENGINE_DEBUG_MODE 1
#   else
#       define ENGINE_DEBUG_MODE 0
#   endif


#	ifdef BUILD_STATIC
#		define _ApiExport
#	else 
#		ifdef BUILD_DLL
#			define _ApiExport __declspec( dllexport )
#		else     
#			define _ApiExport __declspec( dllimport )
#		endif
#	endif

	
#ifdef ENGINE_WCHAR_T_STRINGS
	typedef std::wstring _StringBase;
#else
	typedef std::string  _StringBase;
#endif

#ifdef ENGINE_WCHAR_T_STRINGS
	typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > _StringStreamBase;
#else
	typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _StringStreamBase;
#endif


#ifndef MAXUINT32
#define MAXUINT32   ((UINT32)~((UINT32)0))
#endif
#ifndef MAXINT32
#define MAXINT32 ((INT32)(MAXUINT32 >> 1))
#endif

#ifndef MININT32
#define MININT32    ((INT32)~MAXINT32)
#endif


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }




	typedef _StringBase String;
	typedef _StringStreamBase StringStream;

	typedef std::vector<String> StringVector;

	typedef unsigned char		uint8;
	typedef signed char			int8;
	typedef unsigned short		uint16;
	typedef signed short		int16;
	typedef unsigned int		uint32;
	typedef signed int			int32;
	typedef signed long long	int64;
	typedef unsigned long long  uint64;
	

	class RenderSystem;
	class Plugin;
	class DynLib;
	class RenderTarget;
	class RenderWindow;
	class Camera;
	class Viewport;
	class GameTime;
	class GameClock;
	class Application;
	class VertexElement;
	class VertexDeclaration;
	class EngineRoot;
	class RenderFactory;
	class RenderStateManager;
	class Light;
	class Resource;
	class ResourceManager;
	class ResourceLoader;

	class HardwareBuffer;
	typedef HardwareBuffer* HardwareBufferPtr;
	
	class EffectVariable;
	typedef EffectVariable* EffectVariablePtr;

	class EffectAnnotation;
	typedef EffectAnnotation* EffectAnnotationPtr;

	class EffectParameter;
	typedef EffectParameter* EffectParameterPtr;

	class RenderTechnique;
	typedef RenderTechnique* RenderTechniquePtr;

	class RenderPass;
	typedef RenderPass* RenderPassPtr;

	class RenderEffect;
	typedef RenderEffect* RenderEffectPtr;

	class Texture;
	typedef Texture* TexturePtr;

	class XMLAttribute;
	typedef XMLAttribute* XMLAttributePtr;

	class XMLNode;
	typedef XMLNode* XMLNodePtr;

	class Material;

	struct RenderSettings;
	



}



#endif