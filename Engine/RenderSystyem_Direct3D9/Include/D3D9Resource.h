#pragma once
#ifndef _D3D9Resource__H
#define _D3D9Resource__H

#include "D3D9Prerequisites.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		class _D3D9Export D3D9Resource
		{
			// Interface.
		public:

			// Called immediately after the Direct3D device has been created.
			virtual void NotifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) {}

			// Called before the Direct3D device is going to be destroyed.
			virtual void NotifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) {}

			// Called immediately after the Direct3D device has entered a lost state.
			// This is the place to release non-managed resources.
			virtual void NotifyOnDeviceLost(IDirect3DDevice9* d3d9Device) {}

			// Called immediately after the Direct3D device has been reset.
			// This is the place to create non-managed resources.
			virtual void NotifyOnDeviceReset(IDirect3DDevice9* d3d9Device) {}

		public:
			D3D9Resource			() {};
			virtual ~D3D9Resource	() {};

		};

	}
}


#endif // _D3D9Resource__H