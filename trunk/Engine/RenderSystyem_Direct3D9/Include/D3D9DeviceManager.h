#pragma once
#ifndef _D3D9DeviceManager__H
#define _D3D9DeviceManager__H

#include "D3D9Prerequisites.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9DeviceManager
		{
		public:
			D3D9DeviceManager();
			~D3D9DeviceManager();


			void				SetActiveDevice					(D3D9Device* device);
			D3D9Device*			GetActiveDevice					();

			void				SetActiveRenderTargetDevice		(D3D9Device* device);
			D3D9Device*			GetActiveRenderTargetDevice		();		

			uint32				GetDeviceCount					();
			D3D9Device*			GetDevice						(uint32 index);

			void				NotifyOnDeviceDestroy			(D3D9Device* device);

			D3D9Device*			GetDeviceFromD3D9Device			(IDirect3DDevice9* d3d9Device);

			void				DestroyInactiveRenderDevices	();

			void				LinkRenderWindow				(D3D9RenderWindow* renderWindow);
			


		private:
	
			typedef std::vector<D3D9Device*>		 DeviceList;
			typedef DeviceList::iterator			 DeviceIterator;
			typedef DeviceList::const_iterator		 ConstDeviceIterator;
			typedef std::vector<D3D9RenderWindow*>	 D3D9RenderWindowList;

			D3D9Device*			SelectDevice		(D3D9RenderWindow* renderWindow, D3D9RenderWindowList& renderWindowsGroup);
			D3D9Driver*			FindDriver			(D3D9RenderWindow* renderWindow);	

#pragma warning(disable : 4251)

			DeviceList								mRenderDevices;		
			D3D9Device*								mActiveDevice;
			D3D9Device*								mActiveRenderWindowDevice;

#pragma warning(default : 4251)
		};
	}
}


#endif // _D3D9DeviceManager__H