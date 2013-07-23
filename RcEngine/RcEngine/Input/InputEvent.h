#ifndef InputEvent_h__
#define InputEvent_h__

#include <Core/StringHash.h>

namespace RcEngine {

#define EVENT(eventID, eventName) static const StringHash eventID(#eventName); namespace eventName
#define PARAM(paramID, paramName) static const ShortStringHash paramID(#paramName)

/// Mouse button pressed.
EVENT(E_MOUSEBUTTONDOWN, MouseButtonDown)
{
	PARAM(P_BUTTON, Button);                // int
	PARAM(P_BUTTONS, Buttons);              // int
	PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Mouse button released.
EVENT(E_MOUSEBUTTONUP, MouseButtonUp)
{
	PARAM(P_BUTTON, Button);                // int
	PARAM(P_BUTTONS, Buttons);              // int
	PARAM(P_QUALIFIERS, Qualifiers);        // int
}

EVENT(E_MOUSEMOVE, MouseMove)
{
	PARAM(P_DX, DX);                        // int
	PARAM(P_DY, DY);                        // int
	PARAM(P_BUTTONS, Buttons);              // int
	PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Mouse wheel moved.
EVENT(E_MOUSEWHEEL, MouseWheel)
{
	PARAM(P_WHEEL, Wheel);                  // int
	PARAM(P_BUTTONS, Buttons);              // int
	PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Key pressed.
EVENT(E_KEYDOWN, KeyDown)
{
	PARAM(P_KEY, Key);                      // int
	PARAM(P_BUTTONS, Buttons);              // int
	PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Key released.
EVENT(E_KEYUP, KeyUp)
{
	PARAM(P_KEY, Key);                      // int
	PARAM(P_BUTTONS, Buttons);              // int
	PARAM(P_QUALIFIERS, Qualifiers);        // int
}

}

#endif // InputEvent_h__
