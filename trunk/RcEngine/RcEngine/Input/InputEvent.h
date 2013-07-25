#ifndef InputEvent_h__
#define InputEvent_h__

#include <Core/Prerequisites.h>
#include <Core/StringHash.h>

namespace RcEngine {

struct InputEventType
{
	enum
	{
		None = 0,
		
		KeyDown,
		KeyUp,
		Char,

		MouseButtonDown,
		MouseButtonUp,

		MouseMove,
		MouseWheel,
		
	};

	enum MouseButtonsMask
	{
		NoButton          = 0x00000000,
		LeftButtonMask    = 0x00000001,
		RightButtonMask   = 0x00000002,
		MiddleButtonMask  = 0x00000004,
		XButton1Mask      = 0x00000008,
		XButton2Mask      = 0x00000010
	};
};

struct MouseButtonEvent
{
    int8_t type;
	int32_t button;
    int16_t x, y;
};

struct MouseMotionEvent
{
	 int8_t type;
	 int32_t buttons;
	 int16_t x, y;
};

struct KeyboardEvent
{
	int8_t type;
	uint16_t key;
};

struct CharEvent
{
	int8_t type;
	uint16_t unicode;
};

union InputEvent
{
	int8_t EventType;
	MouseButtonEvent MouseButton;
	MouseMotionEvent MouseMove;
	KeyboardEvent Key;
	CharEvent Char;
};

}

#endif // InputEvent_h__
