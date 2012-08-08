#include "OISInputSystem.h"
#include "OISInputMouse.h"
#include "OISInputKeyboard.h"
#include "Core/Prerequisites.h"
#include "Core/Context.h"
#include "MainApp/Application.h"
#include "MainApp/Window.h"

namespace RcEngine
{
	namespace Input
	{
		static String Name = String("OIS InputSystem");

		OISInputSystem::OISInputSystem()
		{
			// init ois input system
			OIS::ParamList pl;
			std::ostringstream windowHndStr;
			HWND hwnd = Core::Context::GetSingleton().GetApplication().GetMainWindow()->GetHwnd();
			size_t* p=(unsigned int*)&hwnd;
			windowHndStr <<  *p;
			pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
			pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
			pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
			mInputManager = OIS::InputManager::createInputSystem(pl);

			// Here we just create mouse and keyboard
			mDevices.push_back( new OISInputKeyboard(this) );
			mDevices.push_back( new OISInputMouse(this) );
		}

		OISInputSystem::~OISInputSystem()
		{

		}

		const String& OISInputSystem::Name() const
		{
			return Name();
		}

	}
}