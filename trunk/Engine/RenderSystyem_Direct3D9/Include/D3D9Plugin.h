#pragma once
#ifndef _D3D9Plugin__H
#define _D3D9Plugin__H

#include "D3D9Prerequisites.h"
#include "Plugin.h"

namespace Engine
{
	namespace RenderSystem_Direct3D9
	{
		class _D3D9Export D3D9Plugin : public Plugin
		{
		public:
			D3D9Plugin(void);
			~D3D9Plugin(void);

			virtual const String& GetName() const;

			virtual void Install();

			virtual void Initialise();

			virtual void Shutdown();

			virtual void Uninstall();

		private:
			D3D9RenderSystem* mRenderSystem;
		};
	}
	
}

#endif