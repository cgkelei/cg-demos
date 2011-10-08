#pragma once
#ifndef _D3D9Technique__H
#define _D3D9Technique__H

#include "D3D9Prerequisites.h"

namespace Engine
{	
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9Technique : Technique
		{
		public:
			D3D9Technique(D3D9Effect* effect, D3DXHANDLE handle);
			~D3D9Technique();

		private:
			D3DXHANDLE mTechnique;
			
		};
	}
}
#endif