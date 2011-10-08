#pragma once
#ifndef _D3D9Util__H
#define _D3D9Util__H

// [8/25/2011 hustruan]

#include "D3D9Prerequisites.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		class D3D9Util
		{
		public:
			static String GetErrorDescription(HRESULT hr);

		};


	}
}


#endif