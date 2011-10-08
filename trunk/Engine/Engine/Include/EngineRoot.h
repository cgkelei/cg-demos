#pragma once
#ifndef _EngineRoot__H
#define _EngineRoot__H

#include "Prerequisites.h"
#include "Singleton.h"
#include "Plugin.h"


namespace Engine
{
	typedef std::vector<RenderSystem*> RenderSystemList;


	class _ApiExport EngineRoot : public Singleton<EngineRoot>
	{
	public:
		SINGLETON_DECL_HEADER(EngineRoot)

	public:
		EngineRoot(const String& pluginFileName = "plugins.cfg", 
			const String& configFileName = "engine.cfg");
		~EngineRoot();
		
		void LoadPlugins( const String& pluginsfile = "plugins.cfg" );

	
		/** 在装载相关渲染子系统后，有dllStartPlugin调用
		*/
		void AddRenderSystem(RenderSystem* newRend) { mRenderers.push_back(newRend); }

		/** 装载DLL的时候由DLL中的初始化函数调用
		*/
		void InstallPlugin(Plugin* plugin);

		/** 卸载DLL的时候由DLL中的卸载函数调用
		*/
		void UninstallPlugin(Plugin* plugin);

		
		/** 获取已经转载的渲染子系统
		*/
		const RenderSystemList& GetAvailableRenderers(void) { return mRenderers; }

		/** 根据名字获取渲染子系统
		*/
		RenderSystem* GetRenderSystemByName(const String& name);

		/** 设置当前渲染系统
		*/
		void SetRenderSystem(RenderSystem* system);


		RenderWindow* Initialize(bool autoCreateWindow, const String& windowTitle = "Render Window");

		RenderSystem* GetRenderSystem() { return mActiveRenderer; }

		RenderFactory* GetRenderFactory() { return mActiceRenderFactory; }

	private:
		
		

		/** 根据名称加载插件,加载的时候自动调用DLL中的初始化函数
		*/
		void LoadPlugin(const String& pluginName);

		/** 根据名称卸载插件,卸载的时候自动调用DLL中的卸载函数
		*/
		void UnloadPlugin(const String& pluginName);


	public:
		typedef std::vector<DynLib*> PluginLibList;
		typedef std::vector<Plugin*> PluginInstanceList;

	private:

		bool mIsInitialised;

#   pragma warning (push)
#   pragma warning (disable : 4251)

		RenderSystemList mRenderers;
		PluginLibList mPluginLibs;
		PluginInstanceList mPlugins;

#   pragma warning (pop)

		RenderSystem* mActiveRenderer;
		RenderFactory* mActiceRenderFactory;

		String mConfigFileName;

		
	};
}




#endif
