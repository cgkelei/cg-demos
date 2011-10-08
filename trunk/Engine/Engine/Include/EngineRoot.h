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

	
		/** ��װ�������Ⱦ��ϵͳ����dllStartPlugin����
		*/
		void AddRenderSystem(RenderSystem* newRend) { mRenderers.push_back(newRend); }

		/** װ��DLL��ʱ����DLL�еĳ�ʼ����������
		*/
		void InstallPlugin(Plugin* plugin);

		/** ж��DLL��ʱ����DLL�е�ж�غ�������
		*/
		void UninstallPlugin(Plugin* plugin);

		
		/** ��ȡ�Ѿ�ת�ص���Ⱦ��ϵͳ
		*/
		const RenderSystemList& GetAvailableRenderers(void) { return mRenderers; }

		/** �������ֻ�ȡ��Ⱦ��ϵͳ
		*/
		RenderSystem* GetRenderSystemByName(const String& name);

		/** ���õ�ǰ��Ⱦϵͳ
		*/
		void SetRenderSystem(RenderSystem* system);


		RenderWindow* Initialize(bool autoCreateWindow, const String& windowTitle = "Render Window");

		RenderSystem* GetRenderSystem() { return mActiveRenderer; }

		RenderFactory* GetRenderFactory() { return mActiceRenderFactory; }

	private:
		
		

		/** �������Ƽ��ز��,���ص�ʱ���Զ�����DLL�еĳ�ʼ������
		*/
		void LoadPlugin(const String& pluginName);

		/** ��������ж�ز��,ж�ص�ʱ���Զ�����DLL�е�ж�غ���
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
