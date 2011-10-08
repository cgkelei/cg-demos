#pragma once
#ifndef _D3D9VideoMode__H
#define _D3D9VideoMode__H

//  [8/13/2011 hustruan] 初次创建

#include "D3D9Prerequisites.h"


namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		// 保存显示模式信息
		//////////////////////////////////////////////////////////////////////////

		class _D3D9Export D3D9VideoMode
		{
		public:
			D3D9VideoMode();
			D3D9VideoMode( const D3D9VideoMode &ob ); 
			D3D9VideoMode( D3DDISPLAYMODE d3ddm ) ;
			~D3D9VideoMode();

			D3DDISPLAYMODE GetDisplayMode() const { return mDisplayMode; }

			uint32 GetWidth() const			{ return mDisplayMode.Width; }
			uint32 GetHeight() const		{ return mDisplayMode.Height; }
			uint32 GetRefreshRate() const	{ return mDisplayMode.RefreshRate; }
				
			D3DFORMAT GetFormat() const		{ return mDisplayMode.Format; }

			void IncreaseRefreshRate(unsigned int rr) { mDisplayMode.RefreshRate = rr; } 
			
			inline uint32 GetColourDepth() const;		
			inline String GetDescription() const;


		private:
			D3DDISPLAYMODE mDisplayMode;
			uint32 modeNumber;

		};

	}
}


#endif // _D3D9VideoMode__H