#include "D3D9ResourceManager.h"
#include "D3D9Resource.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{


		D3D9ResourceManager::D3D9ResourceManager()
		{

		}

		D3D9ResourceManager::~D3D9ResourceManager()
		{

		}

		void D3D9ResourceManager::NotifyOnDeviceCreate( IDirect3DDevice9* d3d9Device )
		{	
			ResourceContainerIterator iter = mResources.begin();
			ResourceContainerIterator end = mResources.end();
			while( iter != end )
			{
				(*iter)->NotifyOnDeviceCreate(d3d9Device);
				++iter;
			}
		}

		void D3D9ResourceManager::NotifyOnDeviceDestroy( IDirect3DDevice9* d3d9Device )
		{
			ResourceContainerIterator iter = mResources.begin();
			ResourceContainerIterator end = mResources.end();
			while( iter != end )
			{
				(*iter)->NotifyOnDeviceDestroy(d3d9Device);
				++iter;
			}
		}

		void D3D9ResourceManager::NotifyOnDeviceLost( IDirect3DDevice9* d3d9Device )
		{
			ResourceContainerIterator iter = mResources.begin();
			ResourceContainerIterator end = mResources.end();
			while( iter != end )
			{
				(*iter)->NotifyOnDeviceLost(d3d9Device);
				++iter;
			}
		}

		void D3D9ResourceManager::NotifyOnDeviceReset( IDirect3DDevice9* d3d9Device )
		{
			ResourceContainerIterator iter = mResources.begin();
			ResourceContainerIterator end = mResources.end();
			while( iter != end )
			{
				(*iter)->NotifyOnDeviceReset(d3d9Device);
				++iter;
			}
		}

		void D3D9ResourceManager::NotifyResourceCreated( D3D9Resource* pResource )
		{
			mResources.push_back(pResource);
		}

		void D3D9ResourceManager::NotifyResourceDestroyed( D3D9Resource* pResource )
		{
			ResourceContainerIterator it = mResources.begin();

			while (it != mResources.end())
			{
				if ((*it) == pResource)
				{
					mResources.erase(it);
					break;
				}			
				++it;
			}	
		}
	}
}