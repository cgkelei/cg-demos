#pragma once
#ifndef _D3D9VideoModeList__H
#define _D3D9VideoModeList__H

//  [8/13/2011 hustruan] 

#include "D3D9Prerequisites.h"
#include "D3D9VideoMode.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9VideoModeList
		{
		public:
			D3D9VideoModeList(D3D9Driver* driver);
			~D3D9VideoModeList(void);
			
			bool Enumerate();

			uint32 Count();

			D3D9VideoMode* Item( uint32 index );
			D3D9VideoMode* Item( const String &name );
			

		private:
		
			D3D9Driver* mpDriver;

#pragma warning (disable : 4251)
		
			std::vector<D3D9VideoMode> mModeList;
			
#pragma warning (default : 4251)
		};

	}
}


#endif // _D3D9VideoModeList__H

