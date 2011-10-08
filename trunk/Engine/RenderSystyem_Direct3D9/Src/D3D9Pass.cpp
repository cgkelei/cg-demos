#include "D3D9Pass.h"
#include "D3D9Effect.h"
#include "D3D9Technique.h"


namespace Engine
{
	namespace RenderSystem_Direct3D9
	{

		D3D9Pass::D3D9Pass()
		{

		}

		D3D9Pass::~D3D9Pass()
		{

		}

		bool D3D9Pass::Apply( uint32 flag )
		{
			D3D9Effect* mEffect = static_cast<D3D9Effect*>(mParent->GetParent());
			

		}
	}
}