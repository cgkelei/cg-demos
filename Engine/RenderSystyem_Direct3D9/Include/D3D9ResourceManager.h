#pragma once
#ifndef _D3D9ResourceManager__H
#define _D3D9ResourceManager__H

#include "D3D9Prerequisites.h"


namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9ResourceManager
		{
			// Interface.
		public:

			// Called immediately after the Direct3D device has been created.
			void NotifyOnDeviceCreate	(IDirect3DDevice9* d3d9Device);

			// Called before the Direct3D device is going to be destroyed.
			void NotifyOnDeviceDestroy	(IDirect3DDevice9* d3d9Device);

			// Called immediately after the Direct3D device has entered a lost state.
			void NotifyOnDeviceLost		(IDirect3DDevice9* d3d9Device);

			// Called immediately after the Direct3D device has been reset.
			void NotifyOnDeviceReset	(IDirect3DDevice9* d3d9Device);

			D3D9ResourceManager			();
			~D3D9ResourceManager		();	

		private:

			// Called when new resource created.
			void NotifyResourceCreated		(D3D9Resource* pResource);

			// Called when resource is about to be destroyed.
			void NotifyResourceDestroyed	(D3D9Resource* pResource);


		private: 
			typedef std::vector<D3D9Resource*> ResourceContainer;
			typedef ResourceContainer::iterator ResourceContainerIterator;

#pragma warning(disable : 4251)

			ResourceContainer			mResources;

#pragma warning(default: 4251)

		};
	}
}


#endif // _D3D9ResourceManager__H