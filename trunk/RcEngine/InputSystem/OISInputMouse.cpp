#include "OISInputMouse.h"
#include "OISInputSystem.h"

namespace RcEngine
{
	namespace Input
	{
		String MouseName = String("OIS Mouse");

		OIS::MouseButtonID MapToOIS(MouseCode button) 
		{
			switch(button)
			{
			case MS_LeftButton:
				return OIS::MB_Left;
			case MS_RightButton:
				return OIS::MB_Right;
			case MS_MiddleButton:
				return OIS::MB_Middle;
			default:
				return OIS::MB_Left;
			}
		}

		OISInputMouse::OISInputMouse( InputSystem* inputSystem, uint32_t width, uint32_t height )
		{
			assert(inputSystem);
			OIS::InputManager* inputManager = (static_cast<OISInputSystem*>(mInputSystem))->GetInputManager();
			mMouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, false));
			
			// feed window width, height
			const OIS::MouseState &ms = mMouse->getMouseState();
			ms.width = width;
			ms.height = height;
		}

		OISInputMouse::~OISInputMouse()
		{

		}

		long OISInputMouse::X() const
		{
			return mMouse->getMouseState().X.rel;
		}

		long OISInputMouse::Y() const
		{
			return mMouse->getMouseState().Y.rel;
		}

		bool OISInputMouse::ButtonDown( MouseCode button ) const
		{
			return mMouse->getMouseState().buttonDown(MapToOIS(button));
		}

		
		const String& OISInputMouse::GetName() const
		{
			return MouseName;
		}

		void OISInputMouse::Update( float delta )
		{

		}

	}
}