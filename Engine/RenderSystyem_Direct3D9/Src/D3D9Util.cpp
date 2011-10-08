#include "D3D9Util.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		String D3D9Util::GetErrorDescription( HRESULT hr )
		{
			const String errMsg = DXGetErrorDescription( hr );
			return errMsg;	
		}
	}
}