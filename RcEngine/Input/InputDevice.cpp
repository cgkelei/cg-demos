#include "Input/InputDevice.h"
#include "Input/InputSystem.h"


namespace RcEngine{


//---------------------------------------------------------------------------------
Mouse::Mouse( InputSystem* inputSystem )
	: mInputSystem(inputSystem)
{
}

Mouse::~Mouse()
{

}

bool Mouse::ButtonDown( MouseCode button ) const
{
	return ((mButtons[mIndex]) & (1L<<(button-MS_LeftButton))) != 0;
}

bool Mouse::ButtonPress( MouseCode button ) const
{
	uint32_t shift = 1L<<(button-MS_LeftButton);
	return (((mButtons[mIndex]) & shift) != 0) && (((mButtons[!mIndex]) & shift) == 0);
			
}


Keyboard::Keyboard( InputSystem* inputSystem )
	: mInputSystem(inputSystem), mIndex(0)
{

}

Keyboard::~Keyboard()
{

}

bool Keyboard::KeyDown( KeyCode key ) const
{
	return mKeyBuffer[mIndex][key];
}

bool Keyboard::KeyPress( KeyCode key ) const
{
	return (mKeyBuffer[mIndex][key] && !mKeyBuffer[!mIndex][key]);
}

}
