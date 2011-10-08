#pragma  once
#ifndef _D3D9Prerequisites__H
#define _D3D9Prerequisites__H

#include "Prerequisites.h"
#include <Windows.h>
#include <d3dx9.h>
#include <DxErr.h>


#ifdef _DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif

#pragma comment(lib, "D3D9.lib")
#pragma comment(lib, "DxErr.lib")

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

#ifndef BUILD_STATIC
#	ifdef D3DENGINEDLL_EXPORTS
#		define _D3D9Export __declspec(dllexport)
#	else
#    	define _D3D9Export __declspec(dllimport) 
#	endif
#else
#	define _D3D9Export
#endif	
	
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }



		class D3D9RenderSystem;
		class D3D9RenderWindow;
		class D3D9Device;
		class D3D9Driver;
		class D3D9Resource;
		class D3D9ResourceManager;
		class D3D9DeviceManager;
		class D3D9VideoMode;
		class D3D9VideoModeList;
		class D3D9DriverList;
		class D3D9RenderStateManager;
		class D3D9VertexDeclaration;

	}
}


#endif // _D3D9Prerequisites__H
