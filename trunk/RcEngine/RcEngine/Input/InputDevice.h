#ifndef InputDevice_h__
#define InputDevice_h__

#include "Core/Prerequisites.h"

namespace RcEngine
{
	namespace Input
	{
		struct MouseState 
		{
			int32_t x;				
			int32_t y;				
			bool buttons[8];	
		};

		class InputDevice 
		{
		protected:
			InputDevice();

		public:
			virtual ~InputDevice();

			virtual void Update() = 0;

			virtual const uint8_t* GetKeyboardState() = 0;

			virtual const MouseState& GetMouseState() = 0;

			virtual void WarpMouse(uint32_t xPos, uint32_t yPos) = 0;

			virtual void EnableCursor(bool bEnabled) = 0;
		};
	}
}


#endif // InputDevice_h__