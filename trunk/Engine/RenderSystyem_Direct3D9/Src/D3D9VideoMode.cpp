#include "D3D9VideoMode.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		static unsigned int modeCount = 0;

		D3D9VideoMode::D3D9VideoMode()
		{
			modeNumber = ++modeCount;
			ZeroMemory( &mDisplayMode, sizeof(D3DDISPLAYMODE) );
		}

		D3D9VideoMode::D3D9VideoMode( const D3D9VideoMode &ob )
		{
			modeNumber = ++modeCount;
			mDisplayMode = ob.mDisplayMode;
		}

		D3D9VideoMode::D3D9VideoMode( D3DDISPLAYMODE d3ddm )
		{
			modeNumber = ++modeCount; mDisplayMode = d3ddm; 
		}

		D3D9VideoMode::~D3D9VideoMode()
		{
			modeCount--;
		}

		uint32 D3D9VideoMode::GetColourDepth() const
		{
			uint32 colourDepth = 16;
			if( mDisplayMode.Format == D3DFMT_X8R8G8B8 ||
				mDisplayMode.Format == D3DFMT_A8R8G8B8 ||
				mDisplayMode.Format == D3DFMT_R8G8B8 )
				colourDepth = 32;

			return colourDepth;
		}

		String D3D9VideoMode::GetDescription() const
		{
			StringStream stream;
			uint32 colourDepth = 16;
			if( mDisplayMode.Format == D3DFMT_X8R8G8B8 ||
				mDisplayMode.Format == D3DFMT_A8R8G8B8 ||
				mDisplayMode.Format == D3DFMT_R8G8B8 )
				colourDepth = 32;

			stream << mDisplayMode.Width << "X" << mDisplayMode.Height << "@" << colourDepth << "bit color";

			return stream.str();
		}

		
	}
}