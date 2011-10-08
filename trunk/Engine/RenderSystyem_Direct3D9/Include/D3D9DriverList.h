#pragma once
#ifndef _D3D9DriverList__H
#define _D3D9DriverList__H

#include "D3D9Prerequisites.h"
#include "D3D9Driver.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		class _D3D9Export D3D9DriverList
		{
		public:
			D3D9DriverList();
			~D3D9DriverList();

			bool Enumerate();
			uint32 Count() const;
			D3D9Driver* Item( uint32 index );
			D3D9Driver* Item( const String &name );

		private:
#pragma warning (disable : 4251)

			std::vector<D3D9Driver> mDriverList;

#pragma warning (default : 4251)

		};
	}
}



#endif // _D3D9DriverList__H