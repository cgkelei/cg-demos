#include "D3D9Technique.h"
#include "D3D9Effect.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		D3D9Technique::D3D9Technique( D3D9Effect* effect, D3DXHANDLE handle )
			: RenderTechnique(effect),
			  mTechnique(handle)
		{
			
		}

		D3D9Technique::~D3D9Technique()
		{

		}
	}
}