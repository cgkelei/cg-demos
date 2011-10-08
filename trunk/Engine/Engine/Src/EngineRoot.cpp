#include "EngineRoot.h"
#include "DynLibManager.h"
#include "RenderSystem.h"
#include "Exception.h"
#include "ContextCfg.h"

namespace Engine
{
	SINGLETON_DECL(EngineRoot)
	
	typedef void (*DLL_START_PLUGIN) ( void );
	typedef void (*DLL_STOP_PLUGIN) ( void );

	EngineRoot::EngineRoot( const String& pluginFileName /*= "plugins.cfg"*/, const String& configFileName /*= "engine.cfg"*/ )
		: mConfigFileName(configFileName), 
		 mIsInitialised(false),
		 mActiveRenderer(NULL)
	{
		
		DynLibManager::Initialize();
	
		ContextCfg::Initialize();
		ContextCfg::GetSingleton().BuildDefaultConfig();
		

		// Load plugins
		if (!pluginFileName.empty())
			LoadPlugins(pluginFileName);

	}

	EngineRoot::~EngineRoot()
	{
		DynLibManager::Finalize();
		

	}

	void EngineRoot::InstallPlugin( Plugin* plugin )
	{
		mPlugins.push_back(plugin);
		plugin->Install();

		// if rendersystem is already initialised, call rendersystem init too
		if (mIsInitialised)
		{
			plugin->Initialise();
		}
	}

	void EngineRoot::UninstallPlugin( Plugin* plugin )
	{
		PluginInstanceList::iterator i = std::find(mPlugins.begin(), mPlugins.end(), plugin);
		if (i != mPlugins.end())
		{
			plugin->Uninstall();
			mPlugins.erase(i);
		}
	}

	void EngineRoot::LoadPlugin( const String& pluginName )
	{

		DynLib* lib = DynLibManager::GetSingletonPtr()->Load(pluginName);
		if( std::find(mPluginLibs.begin(), mPluginLibs.end(), lib) == mPluginLibs.end() )
		{
			mPluginLibs.push_back(lib);
			// Call startup function

			DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->GetSymbol("dllStartPlugin");

			if (!pFunc)
			{
				assert(false);
			}

			// This must call installPlugin
			pFunc();
		}

	}

	void EngineRoot::UnloadPlugin( const String& pluginName )
	{
		PluginLibList::iterator iter;
		for (iter = mPluginLibs.begin(); iter != mPluginLibs.end(); ++iter)
		{
			if( (*iter)->GetName() == pluginName)
			{	
				// Call plugin shutdown
				DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*iter)->GetSymbol("dllStopPlugin");

				if (!pFunc)
				{
					assert(false);
				}
				// this must call uninstallPlugin
				pFunc();

				// Unload library (destroyed by DynLibManager)
				DynLibManager::GetSingleton().Unload(*iter);
				mPluginLibs.erase(iter);

				break;
			}
		}
	}


	RenderSystem* EngineRoot::GetRenderSystemByName( const String& name )
	{
		if (name.empty())
		{
			// No render system
			return NULL;
		}

		RenderSystemList::const_iterator pRend;
		for (pRend = GetAvailableRenderers().begin(); pRend != GetAvailableRenderers().end(); ++pRend)
		{
			RenderSystem* rs = *pRend;
			if (rs->GetName() == name)
				return rs;
		}

		// Unrecognised render system
		return NULL;
	}

	void EngineRoot::SetRenderSystem( RenderSystem* system )
	{
	
		// Is there already an active renderer?
		// If so, disable it and init the new one
		if( mActiveRenderer && mActiveRenderer != system )
		{
			mActiveRenderer->Shutdown();
		}

		mActiveRenderer = system;

		// Tell scene managers
		//SceneManagerEnumerator::getSingleton().setRenderSystem(system);
	}

	void EngineRoot::LoadPlugins( const String& pluginsfile /*= "plugins.cfg" */ )
	{
		// 后续需要更改
		LoadPlugin("RenderSystem_Direct3D9_d");
		RenderSystem* rs = GetRenderSystemByName("Direct3D9 Rendering Subsystem");
		SetRenderSystem(rs);
	}

	RenderWindow* EngineRoot::Initialize( bool autoCreateWindow, const String& windowTitle /*= "Render Window"*/ )
	{
		if (!mActiveRenderer)
			ENGINE_EXCEPT(Exception::ERR_INVALID_STATE,
			"Cannot initialise - no render "
			"system has been selected.", "Root::initialise");

		//// .rendercaps manager
		//RenderSystemCapabilitiesManager& rscManager = RenderSystemCapabilitiesManager::getSingleton();
		//// caller wants to load custom RenderSystemCapabilities form a config file
		//if(customCapabilitiesConfig != StringUtil::BLANK)
		//{
		//	ConfigFile cfg;
		//	cfg.load(customCapabilitiesConfig, "\t:=", false);

		//	// Capabilities Database setting must be in the same format as
		//	// resources.cfg in Ogre examples.
		//	ConfigFile::SettingsIterator iter = cfg.getSettingsIterator("Capabilities Database");
		//	while(iter.hasMoreElements())
		//	{
		//		String archType = iter.peekNextKey();
		//		String filename = iter.getNext();

		//		rscManager.parseCapabilitiesFromArchive(filename, archType, true);
		//	}

		//	String capsName = cfg.getSetting("Custom Capabilities");
		//	// The custom capabilities have been parsed, let's retrieve them
		//	RenderSystemCapabilities* rsc = rscManager.loadParsedCapabilities(capsName);
		//	if(rsc == 0)
		//	{
		//		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
		//			String("Cannot load a RenderSystemCapability named ") + capsName,
		//			"Root::initialise");
		//	}

		//	// Tell RenderSystem to use the comon rsc
		//	useCustomRenderSystemCapabilities(rsc);
		//}


		return  mActiveRenderer->Initialize(autoCreateWindow, windowTitle);


	}
}