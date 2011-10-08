#pragma once
#ifndef _ContextCfg__H
#define _ContextCfg__H

#include "Prerequisites.h"
#include "RenderSettings.h"
#include "Singleton.h"

namespace Engine
{
	class _ApiExport ContextCfg : public Singleton<ContextCfg>
	{
	public:
		SINGLETON_DECL_HEADER(ContextCfg)
		
		ContextCfg();
		~ContextCfg();

		void LoadConfig(String path);
		void SaveConfig(String path);
		void BuildDefaultConfig();

		String GetRenderSystemName() const { return mRenderSystemName; }
		RenderSettings& GetRenderSettings()  { return mRenderSettings; }


	private:
		String mRenderSystemName;
		RenderSettings mRenderSettings;
	};
}


#endif // _ContextCfg__H