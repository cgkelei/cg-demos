#include "OISInputKeyboard.h"
#include "OISInputSystem.h"

namespace RcEngine
{
	namespace Input
	{
		static String Name = String("OIS Keyboard");

		OISInputKeyboard::OISInputKeyboard( InputSystem* inputSystem )
			: Keyboard(inputSystem)
		{
			assert(inputSystem);
			OIS::InputManager* inputManager = (static_cast<OISInputSystem*>(mInputSystem))->GetInputManager();
			mKeyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
		}

		OISInputKeyboard::~OISInputKeyboard()
		{

		}

		const String& OISInputKeyboard::GetName() const
		{
			return Name;
		}

		void OISInputKeyboard::Update( float delta )
		{
			mKeyboard->capture();
		}

		bool OISInputKeyboard::IsKeyDown( KeyCode key ) const
		{
			return mKeyboard->isKeyDown((OIS::KeyCode)key);
		}

	}
}