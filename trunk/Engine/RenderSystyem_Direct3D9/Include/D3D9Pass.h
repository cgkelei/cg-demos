#pragma once
#ifndef _D3D9Pass__H
#define _D3D9Pass__H

#include "D3D9Prerequisites.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9Pass : RenderPass
		{
		public:
			D3D9Pass();
			~D3D9Pass();
			
		bool Apply(uint32 flag);	

		private:
			UINT mPassNum; // pass number in current technique
		};
	}
}

#endif