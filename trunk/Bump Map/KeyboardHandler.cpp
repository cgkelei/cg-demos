#include "KeyboardHandler.h"

KeyboardHandler::KeyboardHandler(void)
{
}

KeyboardHandler::~KeyboardHandler(void)
{
}

void KeyboardHandler::handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_KEYDOWN)
	{
		setKeyPressed(wParam);
	}
	else if(message == WM_KEYUP)
	{
		setKeyReleased(wParam);

	}
}

bool KeyboardHandler::isKeyPressed(WPARAM wKeyNumber) const 
{
	if (wKeyNumber >= 0 && wKeyNumber <= 256)
	{
		return m_bKeyPressed[wKeyNumber];
	}
	return false;
}

void KeyboardHandler::setKeyPressed(WPARAM wKeyNumber)
{
	if (wKeyNumber >= 0 && wKeyNumber <= 256)
		m_bKeyPressed[wKeyNumber] = true;
}

void KeyboardHandler::setKeyReleased(WPARAM wKeyNumber)
{
	if (wKeyNumber >= 0 && wKeyNumber <= 256)
		m_bKeyPressed[wKeyNumber] = false;
}

KeyboardHandler* KeyboardHandler::Instance()
{
	static KeyboardHandler keyboard;
	return &keyboard;
}
